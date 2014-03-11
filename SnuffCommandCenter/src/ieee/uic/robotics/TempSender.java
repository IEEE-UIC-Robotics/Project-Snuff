package ieee.uic.robotics;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BoxLayout;
import javax.swing.JButton;

public class TempSender extends SnuffCommandModule {
	JButton send;
	byte[] packet = { 'U', 'I', 'C', 1, 'T' };
	
	TempSender(SnuffCommandCenter scc) {
		super(scc);
		getContentPane().setLayout(new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));
		send = new JButton("Send");
		add(send);
		pack();
		setVisible(true);
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		SCC.send(this, packet);
		
	}

	@Override
	public void processResponse(byte[] response) {
		// TODO Auto-generated method stub
		
	}

}