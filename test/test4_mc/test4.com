%link=non-supported
# MC-AFIR
 
0 1
C           -1.20029948081164       -0.94768112074220       -1.17958574123024  1
H           -2.18478839104848       -0.65460851455385       -1.52073515358942  1
C           -0.55772895567366        0.06428845307209       -0.31111876843295  1
O           -1.17646866483181        0.62887889857598        0.56215108576748  1
C           -0.74049761895518       -2.15571201848834       -1.48733771555891  1
H           -1.34491208442627       -2.78803202250495       -2.12580074226719  1
C            0.51711392074563       -2.79188217443321       -1.00522722301005  1
H            0.99534327131514       -2.21504858633832       -0.22117286595239  1
H            0.28901658116777       -3.78446711053971       -0.62020009817583  1
H            1.21505759734568       -2.1670788688016       -1.83379794483712   1
C            0.88486485358940        0.40872664486096       -0.60923048116190  1
H            1.50810858269510        0.12754644541659        0.23819401047334  1
H            1.24241119936533       -0.07807612310579       -1.51002978133948  1
H            0.96395334618935        1.48913220467690       -0.71756567930057  1
C           -2.44073842209445       -4.31350210359259        0.862871752083499 2
H           -3.24627777796774       -4.64500322276891        0.22922630711800  2
C           -2.37912577257444       -3.07380628372055        1.33511355541053  2
H           -1.53216885567676       -2.78038517045963        1.94352129733428  2
C           -3.35228879555377       -2.02009612381081        1.09164429734225  2
H           -2.99708016120657       -1.01637537517131        1.28276519981516  2
C           -4.60246773324293       -2.20493037510571        0.68377560830647  2
H           -5.27141266854680       -1.37661960569494        0.52384462716979  2
H           -5.01114779959403       -3.18607871910812        0.50821904319364  2
H           -1.68321665019442       -5.04529881959268        1.08909442082376  2
Options
NoFC
sublink=grrm2xtb
NFault = 100
Add Interaction
Fragm.1 = 1-14
Fragm.2 = 15-24
1 2
GAMMA = 300
END
