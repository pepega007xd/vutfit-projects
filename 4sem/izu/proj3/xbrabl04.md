node1 [label="obdobi|{teplota=0.1968|den=0.1957|obdobi=0.3058|vhkost=0.1110}"]
node1 -> node2 [label="jaro {6,8,9,11,15}"]
node2 [label="teplota|{teplota=0.8200|den=0.1710|vhkost=0.7219}"]
node2 -> node3 [label="nizka {15}"]
node3 [label="Nizka"]
node2 -> node4 [label="stredni {8,9,11}"]
node4 [label="vhkost|{den=0.2516|vhkost=0.9183}"]
node4 -> node5 [label="mala {11}"]
node5 [label="Velka"]
node4 -> node6 [label="velka {8,9}"]
node6 [label="Stredni"]
node2 -> node7 [label="vysoka {6}"]
node7 [label="Stredni"]
node1 -> node8 [label="leto {10}"]
node8 [label="Velka"]
node1 -> node9 [label="podzim {1,5,7,12,13,14}"]
node9 [label="den|{teplota=0.4591|den=0.6667|vhkost=0.0000}"]
node9 -> node10 [label="pracovni {1,7,14}"]
node10 [label="teplota|{teplota=1.5850|vhkost=0.0000}"]
node10 -> node11 [label="nizka {7}"]
node11 [label="Stredni"]
node10 -> node12 [label="stredni {1}"]
node12 [label="Velka"]
node10 -> node13 [label="vysoka {14}"]
node13 [label="Nizka"]
node9 -> node14 [label="vikend {5,13}"]
node14 [label="Nizka"]
node9 -> node15 [label="svatek {12}"]
node15 [label="Stredni"]
node1 -> node16 [label="zima {2,3,4}"]
node16 [label="teplota|{teplota=0.9183|den=0.2516|vhkost=0.0000}"]
node16 -> node18 [label="stredni {3,4}"]
node18 [label="Stredni"]
node16 -> node19 [label="vysoka {2}"]
node19 [label="Velka"]
