/*
CaesarCipherServer.java
CS 232 Client-Server Computing Project

Created by Ben DeWeerd on 4.18.2022

The CaesarCipherServer class creates a web server to interact with
CaesarCipherClient objects. After establishing a connection and
determining the shift amount, the server accepts input, encodes
it using the Caesar Cipher, and returns the encoded version.

The server uses the standard Socket and ServerSocket classes 
for networking and spawns threads to handle multiple clients
at a time.

The tutorials found here were very helpful:
https://www.infoworld.com/article/2853780/socket-programming-for-scalable-systems.html
*/

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.text.SimpleDateFormat;  
import java.util.Date;  


public class CaesarCipherServer extends Thread {
    private ServerSocket serverSocket;
    private int port;
    private boolean running = false;

    public CaesarCipherServer(int port) {
        this.port = port;
    }

    public void startServer() {
        try {
            serverSocket = new ServerSocket(port);
            this.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void stopServer() {
        running = false;
        this.interrupt();
    }

    @Override
    public void run() {
        running = true;
        while (running) {
            try {
                // receive the next connection
                Socket socket = serverSocket.accept();

                // create new requestHandler thread for new client
                RequestHandler requestHandler = new RequestHandler(socket);
                requestHandler.start();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public static void main(String[] args) {
        if (args.length == 0) {
            System.out.println("Usage: CaesarCipherServer <port>");
            System.exit(0);
        }
        int port = Integer.parseInt(args[0]);

        System.out.println("\n***********************************************************************");
        System.out.println("This program creates a server to communicate with CaesarCipher Clients.");
        System.out.println("Starting server on port: " + port);
        System.out.println("***********************************************************************\n");

        CaesarCipherServer server = new CaesarCipherServer(port);
        server.startServer();
    }
}

class RequestHandler extends Thread {
    private Socket socket;
    private SimpleDateFormat formatter;
    private Date date;

    RequestHandler(Socket socket) {
        this.socket = socket;
        this.formatter = new SimpleDateFormat("MM/dd/yyyy HH:mm:ss");
    }

    @Override
    public void run() {
        try {
            // thread has started with a new client
            date = new Date();
            String clientIP = socket.getRemoteSocketAddress().toString();
            System.out.println("\n" + formatter.format(date) + ": Received a connection.");
            System.out.println("\tClient IP: " + clientIP);

            // get input and output streams
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            PrintWriter out = new PrintWriter(socket.getOutputStream());

            int shift = 0;
            try {
                // get shift amount
                shift = Integer.parseInt(in.readLine());
                if(shift > 25 || shift < 1){
                    throw new Exception("Bad range");
                }
                out.println("Confirmed shift: " + shift);
                out.flush();
            } catch (Exception e) {
                out.println("Error: shift must be in integer, 1-25 inclusive. Exiting.");
                out.flush();
                in.close();
                out.close();
                socket.close();
                date = new Date();
                System.out.println("\n" + formatter.format(date) + ": Connection closed.");
                System.out.println("\tClient IP: " + clientIP);
                return;
            }

            date = new Date();
            System.out.println("\n" + formatter.format(date) + ": shift = " + shift);
            System.out.println("\tClient IP: " + clientIP);

            // read user input, encode it, and send the result until client disconnects
            String line;
            while(true){
                line = in.readLine();
                if(line == null){
                    // client disconnected
                    break;
                }
                line = encode(line, shift);
                out.println(line);
                out.flush();
            }

            // close connection
            in.close();
            out.close();
            socket.close();
            date = new Date();
            System.out.println("\n" + formatter.format(date) + ": Connection closed.");
            System.out.println("\tClient IP: " + clientIP);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // encode string using Caesar Cipher
    private String encode(String line, int shift){
        // see https://stackoverflow.com/questions/19108737/java-how-to-implement-a-shift-cipher-caesar-cipher#:~:text=Add%20shift%20to%20the%20character,Return%20the%20string.
        StringBuilder strBuilder = new StringBuilder();
        char c;
        // iterate through the string, shifting each character individually
        for(int i = 0; i < line.length(); i++){
            c = line.charAt(i);
            if(Character.isLetter(c)){
                c = (char)(line.charAt(i) + shift);
            }
            // wrap around to beginning of alphabet if necessary
            if((Character.isLowerCase(line.charAt(i)) && c > 'z') || (Character.isUpperCase(line.charAt(i)) && c > 'Z')){
                c = (char)(line.charAt(i) - (26 - shift));
            }
            strBuilder.append(c);
        }
        return strBuilder.toString();
    }
}