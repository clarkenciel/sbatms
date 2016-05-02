s.boot;
(
Ndef(\bitter, { |pan = 0, gain = 0.1|
	var words = Array.fill(5, {10.rand});
	var t = PulseCount.ar(Impulse.ar(9e3));
	var x = 1, y = 1;
	words.postln;
	
	Out.ar(0,
		Pan2.ar(
			HPF.ar((
				((x = x | (t & words[0])) & (y = y | (t | words[1])))
				|
				((x + words[2]) * (y + words[3]))
				- words[4] % 255
			) / 127-1,
				20
			).tanh,
			pan, gain)
		);
}).play;
)
Ndef(\bitter).clear;