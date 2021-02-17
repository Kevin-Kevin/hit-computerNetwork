import java.net.ServerSocket;
import java.net.Socket;

import java.util.*;

import jdk.internal.org.objectweb.asm.commons.SerialVersionUIDAdder;
class server {
	public void main() {
		Socket serverSocket;
		try {
			serverSocket = new Socket("0.0.0.0", 80);
		} catch (Exception e) {
			//TODO: handle exception
			System.out.println("error:" + e);
		}
		while (true) {
			serverSocket.
		}
	}
}