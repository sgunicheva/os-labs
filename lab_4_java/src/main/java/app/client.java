package app;

import java.io.*;
import java.net.*;
import java.util.Scanner;

public class client {
    public static void main(String[] args) {
        final String host = "127.0.0.1";
        final int port = 1111;

        try (Socket socket = new Socket(host, port);
             BufferedReader in = new BufferedReader(
                     new InputStreamReader(socket.getInputStream()));
             BufferedWriter out = new BufferedWriter(
                     new OutputStreamWriter(socket.getOutputStream()));
             Scanner scanner = new Scanner(System.in)) {

            System.out.println("Success connect to server.");

            while (true) {
                String message = scanner.nextLine();

                // Отправка
                out.write(message + "\n");
                out.flush();

                // Получение
                String response = in.readLine();
                if (response == null) {
                    System.out.println("Server disconnected.");
                    break;
                }
                System.out.println(response);
            }

        } catch (IOException e) {
            System.err.println("Connection failed: " + e.getMessage());
        }
    }
}
