(
SynthDef(\bitter, { |pan = 0, gain = 0.5|
	var words = Control.names(\words).kr(Array.fill(5,0));
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
}).add;
)

(
~makeSbatm = { |gain = 0.5, pan = 0|
	Environment.make({
		~words = [9,7,8,4,6];
		~synth = nil;
		~gain = gain;
		~pan = pan;
		~play = {
			if(~synth.isNil,
				{ ~synth = Synth(\bitter, [\words,~words,\gain,~gain,\pan,~pan]) },
				{ ~synth })
		};
		~off = {
			if(~synth.isNil, {nil}, {~synth.free})
		};
		~update = {
			if(~synth.isNil,
				{ ~play.value },
				{ ~synth.set(\words, ~words) })
		};
	});
};

~convergeSingle = { |sbatm1, sbatm2, mutationIdx|
	var dir = (sbatm1.use({~words[mutationIdx]}) -
		sbatm2.use({~words[mutationIdx]}) +
		1.0.rand // some noise
	).sign;
	sbatm1.use({~words[mutationIdx] = {
		var res = ~words[mutationIdx] - dir;
		if (res >= 10, { 10 },
			{ if (res <= 1, { 1 },
				{ res })
			});
	}.value;
	});
	sbatm2.use({~words[mutationIdx] = {
		var res = ~words[mutationIdx] + dir;
		if (res >= 10, { 10 },
			{ if (res <= 1, { 1 },
				{ res })
			});
	}.value;
	});
};

~convergeAll = { |sbatm1, sbatm2|
	sbatm1.use({~words.size}).do { |idx|
		~convergeSingle.value(sbatm1, sbatm2, idx);
	};
};
)

(
Tdef(\sbatmSingleModel, {
	// setup
	var numSbatms = 2;
	var sbatms = Array.fill(numSbatms, { |x|
		var gain = 0.8 / numSbatms;
		var pos =  ((x / numSbatms) * 2) - 1;
		~makeSbatm.value(gain,pos);
	});
	var idxArray = Array.fill(sbatms.size, {|idx| idx});

	// start playing
	sbatms.do { |sbatm| sbatm.use({ ~play.value }) };

	// do mutations
	loop {		
		var sbatmIdx1 = idxArray.choose;
		var sbatmIdx2 = idxArray.rotate(-1*sbatmIdx1)[1..].choose;
		var mutationIdx = sbatms[sbatmIdx1].use({ ~words.size.rand });

		~convergeSingle.value(sbatms[sbatmIdx1], sbatms[sbatmIdx2], mutationIdx);
		~convergeAll.value(sbatms[sbatmIdx1], sbatms[sbatmIdx2]);
		sbatms[sbatmIdx1].use({ ~update.value });
		sbatms[sbatmIdx2].use({ ~update.value });

		[mutationIdx,
			[sbatmIdx1, sbatms[sbatmIdx1].use({~words})],
			[sbatmIdx2, sbatms[sbatmIdx2].use({~words})]].debug("mutated");
		0.2.wait;
	}
});
)

Tdef(\sbatmSingleModel).play;

(
Tdef(\sbatmSingleModel).stop;
Server.freeAll;
)

Tdef(\sbatmSingleModel).clear;
Tdef.removeAll;