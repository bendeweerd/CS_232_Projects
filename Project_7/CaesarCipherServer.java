import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

public class CaesarCipherServer extends Thread
{
    private ServerSocket serverSocket;
    private int port;
    private boolean running = false;

    public CaesarCipherServer(int port)
    {
        this.port = port;
    }

    public void startServer()
    {
        try{
            serverSocket = new ServerSocket(port);
            this.start();
        }
        catch(IOException e)
        {
            e.printStackTrace();
        }
    }

    public void stopServer()
    {
        running = false;
        this.interrupt();
    }

    @Override
    public void run()
    {
        running = true;
        while(running)
        {
            try
            {
                System.out.println("Listening for a connection...");
                
                //receive the next connection
                Socket socket = serverSocket.accept();

                // pass socket to RequestHandler thread for processing
                RequestHandler requestHandler = new RequestHandler(socket);
                requestHandler.start();
            }
            catch(IOException e)
            {
                e.printStackTrace();
            }
        }
    }

    public static void main(String[] args)
    {
        if(args.length == 0)
        {
            System.out.println("Usage: CaesarCipherServer <port>");
            System.exit(0);
        }
        int port = Integer.parseInt(args[0]);
        System.out.println("Start server on port: " + port);

        CaesarCipherServer server = new CaesarCipherServer(port);
        server.startServer();

        //TODO: decide when to shutdown server
        try{
            Thread.sleep(60000);
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }

        server.stopServer();
    }
}

class RequestHandler extends Thread
{
    private Socket socket;
    RequestHandler(Socket socket)
    {
        this.socket = socket;
    }

    @Override
    public void run()
    {
        try{
            System.out.println("Received a connection.");

            //get input and output streams
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            PrintWriter out = new PrintWriter(socket.getOutputStream());

            //write out header to the client
            int shift;
            try{
                shift = Integer.parseInt(in.readLine());
                out.println(shift);
            }
            catch(Exception e){
                e.printStackTrace();
            }

            String line = in.readLine();
            do{
                line = in.readLine();
                out.println("Echo: " + line);
                out.flush();
            }while(line != null && line.length() > 0);

            //close connection
            in.close();
            out.close();
            socket.close();
            System.out.println("Connection closed");
        }
        catch(Exception e){
            e.printStackTrace();
        }
    }
}