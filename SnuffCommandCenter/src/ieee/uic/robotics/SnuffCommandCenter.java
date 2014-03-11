package ieee.uic.robotics;

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
	private JMenuItem scanPortsMenuItem;
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
		
		String msg = "";
		while (msg != null) {
			p("SEND: ");
			msg = in.nextLine();
			if ("kill".equals(msg)) {
				serial.disconnect();
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
	public void handleSerialError(String info) {
		pl("SERIAL ERROR: " + info + ")");
	}
	
	@Override
	public void handleSerialData(byte[] data) {
		pl("DATA RECEIVED: " + new String(data));
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		Object source = e.getSource();
		if (source == scanPortsMenuItem) {
			updateConnectionMenu(null);
		}
		else if (source == disconnectMenuItem) {
			serial.disconnect();
			updateConnectionMenu("CONNECT");
			
		}
		else {
			for (int i = 0; i < portMenuItems.size(); i++) {
				JMenuItem portMenuItem = portMenuItems.get(i);
				if (source == portMenuItem)
					if (serial.connect(portMenuItem.getText(), WINDOW_TITLE)) {
						connectionMenu.setText(portMenuItem.getText());
						connectionMenu.removeAll();
						portMenuItems.clear();
						connectionMenu.add(disconnectMenuItem);
						break;
					}
			}
		}
	}
	
	private void updateConnectionMenu(String newMenuText) {
		if (newMenuText != null)
			connectionMenu.setText(newMenuText);
		connectionMenu.removeAll();
		portMenuItems.clear();
		connectionMenu.add(scanPortsMenuItem);
		connectionMenu.addSeparator();
		List<String> names = serial.getPortNames();
		for (String name : names) {
			JMenuItem newPortMenuItem = new JMenuItem(name);
			newPortMenuItem.setFont(new Font(newPortMenuItem.getFont().getName(), Font.BOLD, 20));
			newPortMenuItem.addActionListener(this);
			connectionMenu.add(newPortMenuItem);
			portMenuItems.add(newPortMenuItem);
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
			connectionMenu = new JMenu("CONNECT");
				disconnectMenuItem = new JMenuItem("DISCONNECT");
				scanPortsMenuItem = new JMenuItem("SCAN PORTS");
				portMenuItems = new ArrayList<JMenuItem>(4);
		
		connectionMenu.setFont(new Font(connectionMenu.getFont().getName(), Font.BOLD, 20));
		disconnectMenuItem.setFont(new Font(disconnectMenuItem.getFont().getName(), Font.BOLD, 20));
		scanPortsMenuItem.setFont(new Font(scanPortsMenuItem.getFont().getName(), Font.BOLD, 20));
		
		connectionMenu.setMnemonic( KeyEvent.VK_C );
		disconnectMenuItem.setMnemonic( KeyEvent.VK_D );
		scanPortsMenuItem.setMnemonic( KeyEvent.VK_P );
		
		setJMenuBar(menu);
			menu.add(connectionMenu);
			connectionMenu.add(scanPortsMenuItem);
		
		disconnectMenuItem.addActionListener(this);
		scanPortsMenuItem.addActionListener(this);
	}
	
}
