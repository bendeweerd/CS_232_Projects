/*
CaesarCipherClient.java
CS 232 Client-Server Computing Project

Created by Ben DeWeerd on 4.18.2022

The CaesarCipherClient class creates a client to interact with
CaesarCipherServer objects. After establishing a connection and
determining the shift amount, the client accepts input, sends it
to the server for encoding, receives the server's response, and
displays the result.

The server uses the standard Socket class for networking and 
and the Scanner class to accept user input.

The tutorials found here were very helpful:
https://www.infoworld.com/article/2853780/socket-programming-for-scalable-systems.html
*/

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.net.Socket;
import java.util.Scanner;

class CaesarCipherClient{
    public static void main(String args[])
    {
        if(args.length < 2)
        {
            System.out.println("Usage: CaesarCipherClient <server> <port>");
            System.exit(0);
        }
        String server = args[0];

        System.out.println("\n***********************************************************************");
        System.out.println("This program creates a client to communicate with a CaesarCipherServer.");
        System.out.println("\nBegin by entering the number of places to shift each letter (int, 1-25 inclusive).");
        System.out.println("Then, enter messages to encode.");
        System.out.println("\nType 'quit' and hit enter to exit.");
        System.out.println("***********************************************************************\n");

        System.out.println("Connecting to host: " + server);

        try{
            // connect to server
            Socket socket = new Socket(server, 9876);
            System.out.println("Client IP: " + socket.getLocalSocketAddress().toString() + "\n");

            // create input and output streams to read from and write to the server
            PrintStream out = new PrintStream((socket.getOutputStream()));
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            // scanner to accept user input
            Scanner scanner = new Scanner(System.in);

            // prompt for and send shift
            System.out.print("Enter shift amount: ");
            String shift = scanner.nextLine();
            out.println(shift);
            out.flush();
            
            // read shift confirmation
            String shiftResponse = in.readLine();
            System.out.println(shiftResponse);
            if(shiftResponse.startsWith("Error")){
                in.close();
                out.close();
                socket.close();
                scanner.close();
                System.out.println("Connection closed.");
                return;
            }

            // forever loop; continue processing user input until 'quit'
            while(true){
                System.out.print("\nMessage: ");
                String input = scanner.nextLine();
                if(input.equals("quit")){
                    break;
                }
                out.println(input);
                out.flush();

                // wait for server response, check for disconnect
                String response;
                try{
                    response = in.readLine();
                    if(response.equals(null)){
                        throw new Exception("disconnected");
                    }
                }
                catch(Exception e){
                    System.out.println("Connection lost. Exiting.");
                    in.close();
                    out.close();
                    socket.close();
                    scanner.close();
                    return;
                }

                // display response
                System.out.println("Response: " + response);
            }

            // close streams
            in.close();
            out.close();
            socket.close();
            scanner.close();
            System.out.println("Connection closed.");
        }
        catch(Exception e)
        {
            System.out.println("\nError encountered:");
            e.printStackTrace();
        }
    }
}