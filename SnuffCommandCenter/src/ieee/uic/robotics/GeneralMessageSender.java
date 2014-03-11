package ieee.uic.robotics;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JPanel;

public class GeneralMessageSender extends SnuffCommandModule {
	boolean isSignatureOn = true;
	
	JButton clearButton;
	JButton sendButton;
	JCheckBox sigCheckBox;
	ByteEntryComponent componentOne;
	ArrayList<ByteEntryComponent> componentList;
	
	@SuppressWarnings({ "unchecked", "rawtypes" })
	GeneralMessageSender(SnuffCommandCenter scc) {
		super(scc);
		clearButton = new JButton("Clear");
		sendButton = new JButton("Send");
		sigCheckBox = new JCheckBox("Signature", false);
		componentList = new ArrayList();
		componentOne = new ByteEntryComponent();
		this.add(componentOne);
		this.pack();
		this.setVisible(true);
	}
	
	private class ByteEntryComponent extends JPanel implements ActionListener {
		String msgTypes[] = { "HEX", "DECIMAL" };
		boolean isHex;
		boolean isDec;
		@SuppressWarnings("rawtypes")
		JComboBox msgTypeButton;
		JButton addComponentButton;
		
		ByteEntryComponent() {
			isHex = true;
			isDec = false;
			msgTypeButton = createMsgTypeButton();
			addComponentButton = new JButton();
			this.add(msgTypeButton);
			this.add(addComponentButton);
		}
		
		private JComboBox createMsgTypeButton() {
			@SuppressWarnings({ "rawtypes", "unchecked" })
			JComboBox msgTypeButton = new JComboBox(msgTypes);
			msgTypeButton.setSelectedIndex(0);
			msgTypeButton.addActionListener(this);
			msgTypeButton.setEditable(false);
			return msgTypeButton;
		}
		
		public void actionPerformed(ActionEvent event) {
			JComboBox temp = (JComboBox)event.getSource();
			String msgType = (String)temp.getSelectedItem();
			if(msgType.equals(msgTypes[0])) {
				isHex = true;
				isDec = false;
			}
			else {
				isHex = false;
				isDec = true;
			}
			System.out.println("HEX: " + isHex + " - DEC: " + isDec);
			// updateLabel(msgType);
		}
	}

	@Override
	public void actionPerformed(ActionEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void processResponse(byte[] response) {
		// TODO Auto-generated method stub
		
	}
}
