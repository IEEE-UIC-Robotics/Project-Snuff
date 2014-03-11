package ieee.uic.robotics;

import java.io.*;
import java.util.*;

import gnu.io.*;

// +---------------+ //
// | CLASS: SERIAL | //
// +---------------+ //

public class Serial implements SerialPortEventListener {  
	
	// +-----------+ //
	// | INTERFACE | //
	// +-----------+ //
	
	public interface SerialListener {
		void handleSerialError(String info);
		void handleSerialData(byte[] data);
	}
	
	// +-----------+ //
	// | VARIABLES | //
	// +-----------+ //
	
	public final int BAUD_RATE;
	private final SerialListener listener;
	private SerialPort port;
		
	// +-------------+ //
	// | CONSTRUCTOR | //
	// +-------------+ //
	
	public Serial(int baudrate, SerialListener sl) {
    	BAUD_RATE = baudrate;
    	listener = sl;
    }
    
	// +----------------+ //
	// | PUBLIC METHODS | //
	// +----------------+ //
	
	public List<String> getPortNames() {
		List<String> newPortNames;
		Enumeration<CommPortIdentifier> ports;
		
		newPortNames = new ArrayList<String>(4);
		ports = CommPortIdentifier.getPortIdentifiers();
		
	    while(ports.hasMoreElements())
	    	newPortNames.add(ports.nextElement().getName());
	    
	    return newPortNames;
	}
	
	public boolean connect(String portName, String owner) {
        try {
        	CommPortIdentifier portID;
        	portID = CommPortIdentifier.getPortIdentifier(portName);
			port = (SerialPort) portID.open(owner, 2000);
			port.setSerialPortParams(	BAUD_RATE,
										SerialPort.DATABITS_8,
										SerialPort.STOPBITS_1,
										SerialPort.PARITY_NONE	);
			port.addEventListener(this);
			port.notifyOnDataAvailable(true);
        } catch (Exception e) {
        	if (e instanceof NoSuchPortException)
        		error("PORT " + portName + " NOT FOUND");
        	else if (e instanceof PortInUseException)
        		error("PORT " + portName + " IS ALREADY IN USE");
        	else if (e instanceof UnsupportedCommOperationException)
        		error("FAILED TO SET PORT " + portName + " PARAMETERS");
        	else if (e instanceof TooManyListenersException)
        		error("FAILED TO ADD EVENT LISTENER TO PORT " + portName);
        	return false;
        }
        return true;
    }  
	
	public boolean disconnect() {
		if (port != null) {
			try {
				port.getInputStream().close();
				port.getOutputStream().close();
				port.close();
				port = null;
				return true;
			} catch (IOException e) {
				error("FAILED DURING STREAM OR PORT CLOSING");
			}
		}
		return false;
	}
	
	public boolean write(String data) {
		return write(data.getBytes());
	}
	
	public boolean write(byte[] data) {
		OutputStream out;
        try {
			out = port.getOutputStream();
		} catch (IOException e) {
			error("FAILED TO GET OUTPUT STREAM");
			return false;
		}
		try {
			out.write(data);
		} catch (IOException e) {
			error("FAILED TO WRITE TO OUTPUT STREAM");
			return false;
		}
		try {
	        out.flush();
	    } catch (IOException e) {
	    	error("FAILED TO FLUSH OUTPUT STREAM");
	    	return false;
	    }
	    return true;
	}
	
	@Override
	public void serialEvent(SerialPortEvent e) {
		if (e.getEventType() == SerialPortEvent.DATA_AVAILABLE)
			readSerialData();
	}
	
	// +-----------------+ //
	// | PRIVATE METHODS | //
	// +-----------------+ //

	private boolean readSerialData() {
		InputStream in;
		try {
			in = port.getInputStream();
		} catch (IOException e) {
			error("FAILED TO GET INPUT STREAM");
			return false;
		}
		byte[] buffer = new byte[16];
		ByteArrayOutputStream out = new ByteArrayOutputStream(16);
        try {
        	int received;
			while((received = in.read(buffer)) > 0)
			    out.write(buffer, 0, received);
		} catch (IOException e) {
			error("INPUT STREAM READ FAILED");
			return false;
		}
        listener.handleSerialData(out.toByteArray());
        return true;
	}
	
	private void error(String err) {
		listener.handleSerialError(err);
	}
	
}