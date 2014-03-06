package ieee.uic.robotics;

import java.util.Enumeration;

import gnu.io.*;

public class SnuffCommandCenter implements SerialPortEventListener {

	public static void main(String[] args) {
		
		System.out.println("YEAYAH!!");
		listAllPorts();
	}

	@Override
	public void serialEvent(SerialPortEvent arg0) {
		// TODO Auto-generated method stub
		
	}
	
	private static void listAllPorts() {
		Enumeration ports = CommPortIdentifier.getPortIdentifiers();  
	    
	    while(ports.hasMoreElements())  
	        System.out.println(((CommPortIdentifier)ports.nextElement()).getName());  
	}    
}
