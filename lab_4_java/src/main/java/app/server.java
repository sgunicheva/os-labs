package app;

import java.io.*;
import java.net.*;
import java.util.*;

public class server {

    private static String currentExpression = "";

    public static void main(String[] args) {
        final int port = 1111;

        try (ServerSocket serverSocket = new ServerSocket(port)) {
            System.out.println("Server started. Waiting for client...");
            Socket clientSocket = serverSocket.accept();
            System.out.println("Client connected.");

            BufferedReader in = new BufferedReader(
                    new InputStreamReader(clientSocket.getInputStream()));
            BufferedWriter out = new BufferedWriter(
                    new OutputStreamWriter(clientSocket.getOutputStream()));

            String message;
            while ((message = in.readLine()) != null) {
                System.out.println("From client: " + message);

                switch (message) {
                    case "remove" -> {
                        currentExpression = "";
                        send(out, "Success delete");
                    }
                    case "check" -> {
                        if (checkZero(currentExpression)) {
                            send(out, "Current expression is correct");
                        } else {
                            send(out, "Current expression is incorrect");
                        }
                    }
                    case "=" -> {
                        if (checkZero(currentExpression)) {
                            try {
                                String result = calculate(currentExpression);
                                send(out, result);
                            } catch (Exception e) {
                                send(out, "Error during calculation");
                            }
                        } else {
                            send(out, "Current expression is incorrect");
                        }
                    }
                    default -> {
                        if (tryToAdd(message)) {
                            send(out, "Current: " + currentExpression);
                        } else {
                            send(out, "Incorrect message, try again");
                        }
                    }
                }
            }

        } catch (IOException e) {
            System.err.println("Error: " + e.getMessage());
        }
    }

    private static void send(BufferedWriter out, String message) throws IOException {
        out.write(message + "\n");
        out.flush();
        System.out.println("To client: " + message);
    }

    private static boolean checkZero(String expr) {
        for (int i = 0; i < expr.length() - 1; i++) {
            if (expr.charAt(i) == '/' && expr.charAt(i + 1) == '0') {
                if (i + 2 < expr.length() && expr.charAt(i + 2) == '.') {
                    continue;
                }
                return false;
            }
        }
        return true;
    }

    private static boolean tryToAdd(String input) {
        if (currentExpression.isEmpty()) {
            if (checkNumber(input)) {
                currentExpression = input;
                return true;
            }
            return false;
        }

        if ("+-*/".indexOf(input.charAt(0)) != -1) {
            char op = input.charAt(0);
            String rest = input.substring(1);
            if (checkNumber(rest)) {
                currentExpression += op + rest;
                return true;
            }
        }

        return false;
    }

    private static boolean checkNumber(String num) {
        if (num.startsWith("-")) num = num.substring(1);
        int dotCount = 0;
        for (char c : num.toCharArray()) {
            if (c == '.') {
                dotCount++;
                if (dotCount > 1) return false;
            } else if (!Character.isDigit(c)) return false;
        }
        return true;
    }

    private static int precedence(char op) {
        return switch (op) {
            case '+', '-' -> 1;
            case '*', '/' -> 2;
            default -> 0;
        };
    }

    private static String calculate(String expr) {
        Stack<Character> operators = new Stack<>();
        Queue<String> output = new LinkedList<>();
        StringBuilder number = new StringBuilder();
        boolean lastWasOp = true;

        for (int i = 0; i < expr.length(); i++) {
            char ch = expr.charAt(i);

            if (Character.isDigit(ch) || ch == '.') {
                number.append(ch);
                lastWasOp = false;
            } else if (ch == '-' && lastWasOp) {
                number.append(ch);
            } else {
                if (number.length() > 0) {
                    output.add(number.toString());
                    number.setLength(0);
                }

                while (!operators.isEmpty() && precedence(operators.peek()) >= precedence(ch)) {
                    output.add(Character.toString(operators.pop()));
                }
                operators.push(ch);
                lastWasOp = true;
            }
        }

        if (number.length() > 0) output.add(number.toString());
        while (!operators.isEmpty()) output.add(Character.toString(operators.pop()));

        Stack<Double> values = new Stack<>();
        while (!output.isEmpty()) {
            String token = output.poll();
            if (Character.isDigit(token.charAt(0)) || (token.length() > 1 && token.charAt(0) == '-')) {
                values.push(Double.parseDouble(token));
            } else {
                double b = values.pop();
                double a = values.pop();
                values.push(switch (token) {
                    case "+" -> a + b;
                    case "-" -> a - b;
                    case "*" -> a * b;
                    case "/" -> a / b;
                    default -> throw new RuntimeException("Unknown operator");
                });
            }
        }

        return values.isEmpty() ? "0" : String.valueOf(values.peek());
    }
}

