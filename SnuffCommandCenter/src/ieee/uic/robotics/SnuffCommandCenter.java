package ieee.uic.robotics;

import java.util.*;
import java.util.List;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

@SuppressWarnings("serial")
public class SnuffCommandCenter extends JFrame implements ActionListener, Serial.SerialListener {
	
	public static void main(String[] args) {
		new SnuffCommandCenter();
	}
	
	public static void p(String s) {
		System.out.print(s);
	}
	
	public static void pl(String s) {
		System.out.println(s);
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
	private SnuffCommandModule cmWaitingForResponse;
	
	private JMenu connectionMenu;
	private JMenuItem scanPortsMenuItem;
	private JMenuItem disconnectMenuItem;
	private List<JMenuItem> portMenuItems;
	private JMenu commandModulesMenu;
	private JMenuItem generalMessageSenderMenuItem;
	private JMenuItem tempSenderMenuItem;
	
	// +-------------+ //
	// | CONSTRUCTOR | //
	// +-------------+ //
	
	public SnuffCommandCenter() {
		super(WINDOW_TITLE);
		
		setupGUI();
		
		serial = new Serial(SERIAL_BAUD_RATE, this);
	}
	
	public void send(SnuffCommandModule sender, byte[] packet) {
		if (serial.isConnected()) {
			serial.write(packet);
			if (sender != null)
				cmWaitingForResponse = sender;
		}
	}
	
	@Override
	public void handleSerialError(String info) {
		pl("SERIAL ERROR: " + info + ")");
	}
	
	@Override
	public void handleSerialData(byte[] data) {
		pl("DATA RECEIVED: " + new String(data));
		if (cmWaitingForResponse != null) {
			cmWaitingForResponse.processResponse(data);
			cmWaitingForResponse = null;
		}
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		Object source = e.getSource();
		
		if (source == generalMessageSenderMenuItem) {
			new GeneralMessageSender(this);
		}
		else if (source == tempSenderMenuItem) {
			new TempSender(this);
		}
		else if (source == scanPortsMenuItem) {
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
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setPreferredSize(new Dimension(400, 400));
		getContentPane().setLayout(new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));
		initializeMenuBar();
		pack();
		setLocationRelativeTo(null);
		setVisible(true);
	}
	
	private void initializeMenuBar() {
		JMenuBar menu = new JMenuBar();
			connectionMenu = new JMenu("CONNECT");
				disconnectMenuItem = new JMenuItem("DISCONNECT");
				scanPortsMenuItem = new JMenuItem("SCAN PORTS");
				portMenuItems = new ArrayList<JMenuItem>(4);
			commandModulesMenu = new JMenu("<COMMAND MODULES>");
				generalMessageSenderMenuItem = new JMenuItem("General Message Sender");
				tempSenderMenuItem = new JMenuItem("Temp Sender");
		
		Font menuFont = new Font(connectionMenu.getFont().getName(), Font.BOLD, 20);
		connectionMenu.setFont(menuFont);
		disconnectMenuItem.setFont(menuFont);
		scanPortsMenuItem.setFont(menuFont);
		commandModulesMenu.setFont(menuFont);
		generalMessageSenderMenuItem.setFont(menuFont);
		tempSenderMenuItem.setFont(menuFont);

		setJMenuBar(menu);
			menu.add(connectionMenu);
			menu.add(commandModulesMenu);
			connectionMenu.add(scanPortsMenuItem);
			commandModulesMenu.add(generalMessageSenderMenuItem);
			commandModulesMenu.add(tempSenderMenuItem);
		
		disconnectMenuItem.addActionListener(this);
		scanPortsMenuItem.addActionListener(this);
		generalMessageSenderMenuItem.addActionListener(this);
		tempSenderMenuItem.addActionListener(this);
	}
	
}
