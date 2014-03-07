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
		void handleSerialEvent(SerialEvent event, String info);
		void handleSerialData(byte[] data);
	}
	
	// +----------------+ //
	// | INTERFACE ENUM | //
	// +----------------+ //
	
	public enum SerialEvent {
		PORT_DETECTED, PORT_REMOVED, DATA_RECEIVED, ERROR;
	}
	
	// +-----------+ //
	// | VARIABLES | //
	// +-----------+ //
	
	public final int BAUD_RATE;
	private final SerialListener listener;
	private final List<String> portNames;
	private SerialPort port;
		
	// +-------------+ //
	// | CONSTRUCTOR | //
	// +-------------+ //
	
	public Serial(int baudrate, SerialListener sl) {
    	BAUD_RATE = baudrate;
    	listener = sl;
    	portNames = new ArrayList<String>();
		new Timer().schedule(new PortChecker(), 0, 500);
    }
    
	// +----------------+ //
	// | PUBLIC METHODS | //
	// +----------------+ //
	
	public List<String> getPortNames() {
	    return new ArrayList<String>(portNames);
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
	
	@SuppressWarnings("unchecked")
	private synchronized void checkPorts() {
		List<String> newPortNames;
		Enumeration<CommPortIdentifier> ports;
		
		newPortNames = new ArrayList<String>(Math.max(1, portNames.size()));
		ports = CommPortIdentifier.getPortIdentifiers();
		
	    while(ports.hasMoreElements())
	    	newPortNames.add(ports.nextElement().getName());
	    
	    for (int p = 0; p < portNames.size(); p++) {
	    	String portName = portNames.get(p);
	    	if (newPortNames.remove(portName) == false) {
	    		portNames.remove(p--);
	    		if (port != null && port.getName().equals(portName))
	    			disconnect();
	    		listener.handleSerialEvent(SerialEvent.PORT_REMOVED, portName);
	    	}
	    }
	    
	    for (String portName : newPortNames) {
	    	portNames.add(portName);
	    	listener.handleSerialEvent(SerialEvent.PORT_DETECTED, portName);
	    }
	}
	
	private void error(String err) {
		listener.handleSerialEvent(SerialEvent.ERROR, err);
	}
	
	// +---------------------+ //
	// | CLASS: PORT CHECKER | //
	// +---------------------+ //
	
	private class PortChecker extends TimerTask {
	    public void run() {
	    	checkPorts();
	    }
	}
}  