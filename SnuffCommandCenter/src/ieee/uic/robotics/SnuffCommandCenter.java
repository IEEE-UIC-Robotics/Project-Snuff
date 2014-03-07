package ieee.uic.robotics;

import ieee.uic.robotics.Serial.SerialEvent;

import java.util.*;
import java.util.List;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

@SuppressWarnings("serial")
public class SnuffCommandCenter extends JFrame implements ActionListener, Serial.SerialListener {
	
	public static Scanner in = new Scanner(System.in);
	
	public static void main(String[] args) {
		new SnuffCommandCenter();
	}
	
	// +-----------+ //
	// | CONSTANTS | //
	// +-----------+ //
	
	private static final String
	WINDOW_TITLE = "SNUFF COMMAND CENTER";
	
	private static final int
	SERIAL_BAUD_RATE = 38400;
	
	// +-----------+ //
	// | VARIABLES | //
	// +-----------+ //
	
	private Serial serial;
	private JLabel portsLabel;
	private JMenu connectionMenu;
	private JMenuItem disconnectMenuItem;
	private List<JMenuItem> portMenuItems;
	
	// +-------------+ //
	// | CONSTRUCTOR | //
	// +-------------+ //
	
	public SnuffCommandCenter() {
		super(WINDOW_TITLE);
		
		setupGUI();
		
		serial = new Serial(SERIAL_BAUD_RATE, this);
		
		List<String> portNames = serial.getPortNames();
		
		int i = 0;
		for (String portName : portNames)
			pl(i++ + ": " + portName);
		
		pl("CHOOSE PORT TO OPEN:");
		int portIndex = in.nextInt();
		
		try {
			pl("OPENING PORT " + portIndex + " (" + portNames.get(portIndex) + ")");
			serial.connect(portNames.get(portIndex), WINDOW_TITLE);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		pl("WRITING STUFF");
		serial.write("FUCK YOU\n");
		
		String msg = "";
		while (msg != null) {
			msg = in.nextLine();
			if ("kill".equals(msg)) {
				serial.disconnect();
			}
			else if ("open".equals(msg)) {
				serial.connect(portNames.get(portIndex), WINDOW_TITLE);
			}
			else{
				serial.write(msg);
			}
		}

	}
	
	public static void p(String s) {
		System.out.print(s);
	}
	
	public static void pl(String s) {
		System.out.println(s);
	}

	@Override
	public void handleSerialEvent(SerialEvent event, String info) {
		pl("SERIAL EVENT: " + event + " (" + info + ")");
		if (event == Serial.SerialEvent.PORT_DETECTED ||
			event == Serial.SerialEvent.PORT_REMOVED) {
			connectionMenu.removeAll();
			portMenuItems.clear();
			connectionMenu.add(disconnectMenuItem);
			connectionMenu.addSeparator();
			StringBuilder sb = new StringBuilder(20);
			sb.append("<html>");
			for (String name : serial.getPortNames()) {
				JMenuItem newPortMenuItem = new JMenuItem(name);
				connectionMenu.add(newPortMenuItem);
				portMenuItems.add(newPortMenuItem);
				newPortMenuItem.addActionListener(this);
				sb.append(name);
				sb.append("<br>");
			}
			sb.append("</html>");
			portsLabel.setText(sb.toString());		
		}
	}

	@Override
	public void handleSerialData(byte[] data) {
		pl("DATA RECEIVED: " + new String(data));
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		Object source = e.getSource();
		if (source == disconnectMenuItem)
			serial.disconnect();
		else {
			for (JMenuItem portMenuItem : portMenuItems) {
				if (source == portMenuItem)
					serial.connect(portMenuItem.getText(), WINDOW_TITLE);
			}
		}
	}

	// +-------------+ //
	// | GUI METHODS | //
	// +-------------+ //
	
	private void setupGUI() {
		setPreferredSize(new Dimension(400, 400));
		setResizable(false);
		setLocationRelativeTo(null);
		getContentPane().setLayout(new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));
		getContentPane().add(portsLabel = new JLabel());
		getContentPane().add(new JButton("FAKE"));
		initializeMenuBar();
		pack();
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setLocationRelativeTo(null);
		setVisible(true);
	}
	
	private void initializeMenuBar() {
		JMenuBar menu = new JMenuBar();
			connectionMenu = new JMenu("CONNECTION");
				disconnectMenuItem = new JMenuItem("DISCONNECT");
				portMenuItems = new ArrayList<JMenuItem>(4);
		
		connectionMenu.setFont(new Font(connectionMenu.getFont().getName(), Font.BOLD, 20));
		disconnectMenuItem.setFont(new Font(disconnectMenuItem.getFont().getName(), Font.BOLD, 20));
		
		connectionMenu.setMnemonic( KeyEvent.VK_C );
		disconnectMenuItem.setMnemonic( KeyEvent.VK_D );
		
		setJMenuBar(menu);
			menu.add(connectionMenu);
			connectionMenu.add(disconnectMenuItem);
			connectionMenu.addSeparator();
		
		disconnectMenuItem.addActionListener(this);
	}
	
}
