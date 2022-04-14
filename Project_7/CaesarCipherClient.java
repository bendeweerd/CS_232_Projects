// compile: javac CaesarCipherClient.java
// run:     java CaesarCipherClient

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

        System.out.println("Connecting to host: " + server);
        try{
            // connect to server
            Socket socket = new Socket(server, 9876);

            // create input and output streams to read from and write to the server
            PrintStream out = new PrintStream((socket.getOutputStream()));
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            Scanner scanner = new Scanner(System.in);
            System.out.print("Enter shift amount: ");
            int shift = Integer.parseInt(scanner.nextLine());

            // send input
            out.println(shift);
            
            // read data from server until we finish reading
            String line = in.readLine();
            System.out.println("Confirmed shift: " + line);

            while(true){
                System.out.print("Message: ");
                String input = scanner.nextLine();
                if(input.equals("quit")){
                    break;
                }
                out.println(input);
                String response = in.readLine();
                System.out.println("Response: " + response);
            }

            // close streams
            in.close();
            out.close();
            socket.close();
            scanner.close();
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }
}