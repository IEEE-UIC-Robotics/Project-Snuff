package ieee.uic.robotics;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;

import javax.swing.BoxLayout;
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
	JButton addComponentButton;
	ArrayList<ByteEntryComponent> componentList;
	
	@SuppressWarnings({ "unchecked", "rawtypes" })
	GeneralMessageSender(SnuffCommandCenter scc) {
		super(scc);
		clearButton = new JButton("Clear");
		sendButton = new JButton("Send");
		sigCheckBox = new JCheckBox("Signature", false);
		componentList = new ArrayList();
		componentOne = new ByteEntryComponent();
		addComponentButton = new JButton("+");
		addComponentButton.addActionListener(this);
		getContentPane().setLayout(new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));
		this.add(addComponentButton);
		this.add(componentOne);
		this.add(new ByteEntryComponent());
		this.pack();
		this.setVisible(true);
	}
	
	public void addComponent(ByteEntryComponent comp) {
		this.add(comp);
		this.pack();
	}
	
	private class ByteEntryComponent extends JPanel implements ActionListener {
		String msgTypes[] = { "HEX", "DECIMAL" };
		boolean isHex;
		boolean isDec;
		@SuppressWarnings("rawtypes")
		JComboBox msgTypeButton;
		
		ByteEntryComponent() {
			isHex = true;
			isDec = false;
			msgTypeButton = createMsgTypeButton();
			this.add(msgTypeButton);
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
			Object src = event.getSource();
			if(src == msgTypeButton) {
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
			}
			
			// updateLabel(msgType);
		}
	}

	@Override
	public void actionPerformed(ActionEvent event) {
		// TODO Auto-generated method stub
		Object src = event.getSource();
		if(src == addComponentButton) {
			ByteEntryComponent temp = new ByteEntryComponent();
			//this.remove(addComponentButton);
			this.add(temp);
			pack();
			System.out.println("POOP");
		}
	}

	@Override
	public void processResponse(byte[] response) {
		// TODO Auto-generated method stub
		
	}
}