 
















 








 






 
 
























 























 




 



 












 







 





































:userdoc.
:title.Help 4 Connect
:docprof toc=12.

 



 

:h1 id=6000  res=6000 .How to Play Connect Four
:i1.How to Play Connect Four
:p.
Connect Four is a two-player-game. On his turn, each player inserts a
coin of his colour in one of the columns. When a column is full, no
more insertions are allowed. The player who first manages to get four coins
of his colour in one line (either diagonal, horizontal or vertical)
is the winner. In this game either none, one or both of the players can 
be simulated by the computer. 
.br
The last option (both players are played
by the computer) may not be too thrilling for you, but during
the development of the program it was a good method to compare different
valuation functions. We decided to leave this option in the game, 
maybe in a future release you will be able to select different valuation
functions. If you had the source code, you could do this already &colon.-)
For now, you can use it to check how two computer players 
play at different levels. If you want to get more information about how the
computer plays, see :link reftype=hd refid=PANEL_AI.How the computer
finds a move:elink.. 
:p.
:note.
This program is freely distributed under the usual terms and conditions
for use and distribution of free software. If you are uncertain what
this means refer to
:link reftype=hd refid=6503 .Conditions for Using Connect Four:elink.. 

:p.
Related topics&colon.
:sl compact.
:li. :link reftype=hd refid=6501 .About the Authors:elink.
:li. :link reftype=hd refid=6503 .Conditions for Using Connect Four:elink. 
:li. :link reftype=hd refid=6502 .Getting the Sources:elink.
:esl.

 

:h1 id=6100  res=6100 .Game Options
:i1.Game Options
:p.
You can select the identity (either Human or Computer) of both of 
the players, and you can choose the player who is to begin. Any 
combination is possible. The slider settings determine
the level that the computer plays at. If the player is human, the slider
indicates the level at which the computer will calculate hint moves for 
this player. 
.br
The higher
the level, the better (and slower) the computer plays. As you will find
out quickly, it&apos.s no task to win against the computer when it plays on
a level below four. And it&apos.s no fun to play when the level is above 
seven (because the computer decision takes too long - and you will not
be able to win!).
.br
If you want to know more about the way the computer calculates his moves,
please refer to 
:link reftype=hd refid=PANEL_AI.How the Computer Finds a Move:elink..

:p.Related topics&colon.
:sl compact.
:li. :link reftype=hd refid=6200 .Program Options:elink.
:li. :link reftype=hd refid=PANEL_AI.How the Computer Finds a Move:elink.
:esl.

 

:h1 id=6200  res=6200 .Program Options
:i1.Program Options
:p.
Here you can select some options concerning the whole program&colon.
:dl break=all tsize=3.
:dt.:hp2.Enable Sound:ehp2.
:dd.
To select this option, you need MMPM/2.
:dt.:hp2.Animate:ehp2.
:dd.
Determines whether or not the coins drop visually. When sound is
enabled but animation is not, there will be no sound when you insert
a coin or start a new game.
:dt.:hp2.Change Pointer:ehp2.
:dd.
If selected, the mouse pointer will appear in different shapes 
according to its position and to the state of the game.
:dt.:hp2.Save Settings on Exit:ehp2.
:dd.
If you select this option, all settings, including the window position 
and size, will be saved automatically when the program ends. 
All settings will be saved in the file connect4.ini in the current 
directory.
:edl.

:p.Related topics&colon.
:sl compact.
:li. :link reftype=hd refid=6100 .Game Options:elink.
:esl.

 

:h1 id=6300  res=6300 .Product Information
:p.
This dialog gives you some (boring) information about the authors of
this program. Simply press OK to leave it. 
:p.
Related topics&colon.
:sl compact.
:li. :link reftype=hd refid=6000 .How to Play Connect Four:elink.
:li. :link reftype=hd refid=6501 .About the Authors:elink.
:li. :link reftype=hd refid=6503 .Conditions for Using Connect Four:elink. 
:li. :link reftype=hd refid=6502 .Getting the Sources:elink.
:esl.

 



:h1 id=6040  res=6040 .Help for the Menu Entries
:i1.Help for the Menu Entries
:p.
To get help for the menu entries, you can either choose one of the
topics listed below, or simply press F1 after selecting the menu
entry you want to get information about.
:p.
Menu entries&colon.
:sl compact.
:li. :link reftype=hd refid=6001 .Help for File:elink.
:li. :link reftype=hd refid=6010 .Help for Game:elink.
:li. :link reftype=hd refid=6030 .Help for Options:elink.
:li. :link reftype=hd refid=6020 .Help for Help:elink.
:esl.

  

:h1 id=6001  res=6001 .Help for File
:p.
The following menu items exist:
:dl break=all tsize=3.
:dt.:hp2.Load:ehp2.
:dd.
Restore a previously saved game. The game options (player levels,
starting player etc.) are not stored so that the current options 
will be taken.
:dt.:hp2.Save:ehp2.
:dd.
Saves the current game in connect4.ini. You can restore it later and 
continue playing ...
:dt.:hp2.Exit:ehp2.
:dd.
You may guess what happens if you select this menu item.
:edl.

 

:h1 id=6010  res=6010 .Help for Game
:p.
The following menu items exist:
:dl break=all tsize=3.
:dt.:hp2.Start:ehp2.
:dd.
This menu item is only available at the beginning of a game and only if 
the computer has to make the first move. After selecting this menu item 
the computer will start its calculation to find a good choice for the first
column to insert a coin.
:dt.:hp2.Undo:ehp2.
:dd.
Gives you the possibility to rethink your last decision. 
:dt.:hp2.Hint:ehp2.
:dd.
The computer tells you which move it would select if it were in your place.
:dt.:hp2.New:ehp2.
:dd.
Choosing this point will abort the current game. The game board will be
cleared. If the player who is going to start
(see :link reftype=hd refid=6100 .Game Options:elink.) is the
computer you have to choose :hp2.Start:ehp2. afterwards to begin the
game. If the starting player is human you can drop the first
coin immediately after the board is cleared.
:edl.

 

:h1 id=6030  res=6030 .Help for Options
The following menu items exist:
:dl break=all tsize=3.
:dt.:hp2.Game:ehp2.
:dd.
Here you can select options concerning the computer level and related things.
See also :link reftype=hd refid=6100 .Game Options:elink..
:dt.:hp2.Program:ehp2.
:dd.
The dialog that appears if you select this menu item allows you to enable
or disable some options concerning the appearance of the program. See also
:link reftype=hd refid=6200 .Program Options:elink..
:dt.:hp2.Save:ehp2.
:dd.
Here you can save the current options to the file connect4.ini. 
You also have the opportunity to save the options every time you
quit the program. For this see 
:link reftype=hd refid=6200 .Program Options:elink..
:edl.

 

:h1 id=6020  res=6020 .Help for Help
:p.
The following menu items exist:
:dl break=all tsize=3.
:dt.:hp2.Help Index:ehp2.
:dd.
This brings up a list with more or less interesting information about the
program Connect Four. 
:dt.:hp2.General help:ehp2.
:dd.
This help screen will explain how to play Connect Four.
:dt.:hp2.Product Information:ehp2.
:dd.
This displays a short information about the program and its authors. If your
curiosity is not satisfied concerning this item, you can also refer to the
:link reftype=hd refid=6300 .Product Information:elink.,
and, if even this does not satisfy you, furthermore to
:link reftype=hd refid=6501 .About the Authors:elink.. This should 
do.
:edl.

 



:h1 id=PANEL_AI.How the Computer Finds a Move
:i1.How the Computer Finds a Move
:p.
Here you will find information about the algorithm that calculates
the computer&apos.s choice if it is its turn to insert the next coin. This
is only a brief description. For more information look for books dealing
with "Game Theory", a subsection of Artificial Intelligence. Especially 
if you speak German I strongly
recommend you to read an article in the German magazine "c&apos.t", August 
1989, page 114, called "Dame spielen". Although the game described there is
not Connect Four, the explanation of the methods for this kind of AI
was totally enough to develop this program. This should hold for any other
game that is similar to Connect Four, that is any game where the computer 
has a variable but finite number of possibilities to make his next
move.
:p.The current game status is saved in two identical board structures, one to
display the board and the other for the computer&apos.s calculations. 
In this second board
it can insert coins to test the results of its imaginary moves. 
(It was necessary to have two boards because when the window is updated 
during the calculation,
it should still display the "real" situation on the game board and not
the state of the computer&apos.s calculations.) 
When the computer is going to move, it inserts a coin in a column of "his"
board and
checks if it has won (inserting one coin is called a "half move"). When
it has won, it decides this move to be a good one and therefore will 
also execute it in the "real" board. Normally, this
does not occur too often, so it has to check what will happen after this 
move. It simulates an opponent&apos.s move, checks if the opponent has won,
and so on.
After the computer has 
reached the recursion limit, it will validate the situation displayed on the
game board. These values are returned recursively, and after having checked
all game situations, it will select the column with the best valuation
and execute this move in the real game board.
The level selected for the computer is directly related to the calculation
depth, i. e. after how many of these "half moves" it will stop to recursively
insert coins. This limit is necessary because when one of the players inserts
a coin, the other player normally has seven possibilities to answer this move,
and thus the computer will take seven times longer to calculate when the
level is increased only one step. There are methods how to reduce this (they
will be explained in game theory books or articles), but experience shows
that even with these methods the computing time will be three to four times 
longer when the level is increased one step.  
:note.When you have enabled the sound option and when you play at a level
above four, you will sometimes hear a
laughter. This indicates that the computer knows that he will win no
matter what you do afterwards (you can give up then!). Just a nastiness
towards you&colon.-).
:p.
Related topics&colon.
:sl compact.
:li.:link reftype=hd refid=6000 .How to Play Connect Four:elink.
:li.:link reftype=hd refid=6501 .About the Authors:elink.
:li.:link reftype=hd refid=6503 .Conditions for Using Connect Four:elink. 
:li.:link reftype=hd refid=6502 .Getting the Sources:elink.
:esl.



:h1 id=6501 .About the Authors
:i1.About the Authors
:p.
The program was written by Nicole Greiber and Ralf Seidel. Both are
currently students of mathematics at the BUGH Wuppertal (Bergische
UniversitÑt Gesamthochschule Wuppertal). Wuppertal is a small town
in Germany near Cologne.
:p.
You can reach us via eMail (InterNet):
:p.
:lm margin=4.
greiber@wrcs3.urz.uni-wuppertal.de
:p.
seidel3@wrcs3.urz.uni-wuppertal.de
:lm margin=1.
:p.
or via Snail Mail:
:lines.
:lm margin=4.Ralf Seidel :lm margin=30.Nicole Greiber :lm margin=4.
WÅlfrather Str. 45 :lm margin=30.BrÅningstr. 6 :lm margin=4.
42105 Wuppertal :lm margin=30.42119 Wuppertal :lm margin=4.
Germany :lm margin=30.Germany :lm margin=1.
:elines.
:p.
Any kind of mail is welcome, may it be hints, comments, wishes for future 
releases, bug reports, corrections concerning our English or (less welcome) 
complaints. In one word&colon. Drop us a note, and we&apos.ll be happy!
:p.
Related topics&colon.
:sl compact.
:li.:link reftype=hd refid=6000 .How to Play Connect Four:elink.
:li.:link reftype=hd refid=6503 .Conditions for Using Connect Four:elink. 
:li.:link reftype=hd refid=6502 .Getting the Sources:elink.
:esl.



:h1 id=6502 .Getting the Sources
:i1.Getting the Sources
:p.
If you want to obtain the source codes for this program, we would like
you to send us DM 40,- (or US-$ 25,- likewise). :hp8.PLEASE:ehp8. do 
:hp8.NOT:ehp8. 
send foreign cheques, because the German banks charge about DM 10,- to
change such a cheque, and we guess that once you have decided to pay 
something, you would like to pay us and not the bank.
:p.
Once you have got the source code, you will not get rid of us that quick.
This means that we will drop you a note when we have developed a new
version of Connect Four so that you can get the new sources if you like
(a whole life long!). Obviously, this includes that we store your address.
If you do :hp2.NOT:ehp2. want this, please tell us, and we will not store it.
:p.
To compile the source codes, we have used EMX 0.8h (as you might have guessed
when you found the EMX DLL&apos.s). We did not try to use any other compiler,
but we do not think that it will work very well with the Borland C++ Compiler,
because some functions are defined and used in a different way. It should
be easier to use the IBM compiler, but there might also occur some minor
problems. If you want to compile the source code, the easiest way for
you would be to get and install the EMX compiler on your machine (if you
haven&apos.t done this already). It is FREE!, and it&apos.s available on various 
FTP sites, as for example ftp.cdrom.com or its mirror in Germany at 
ftp.uni-stuttgart.de. If you want to compile the sound features, you also
need a multimedia package (but you don&apos.t need this if you want to compile
without sound). You can either use the mm4emx package written by Marc van
Woerkom (what we did), which is also available
via FTP, or the package included with the Developer&apos.s Toolkit Ver. 2.1.
:p.
Related topics&colon.
:sl compact.
:li.:link reftype=hd refid=6000 .How to Play Connect Four:elink.
:li.:link reftype=hd refid=6503 .Conditions for Using Connect Four:elink. 
:li.:link reftype=hd refid=6501 .About the Authors:elink.
:esl.

:h1 id=6503 .Conditions for Using Connect Four
:i1.Conditions for Using Connect Four
:p.
Copyright (C) 1994 by Nicole Greiber and Ralf Seidel
:p.
This program is distributed in the hope that it will be useful, but
:hp8.WITHOUT ANY WARRANTY:ehp8.; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
:p.
This program is free software; you can redistribute it if you make
sure that anybody who obtained a copy has the same rights as you have.
:ol.
:li.You may copy this program like anything.
:li.It&apos.s not allowed to take money for this program itself. (A fee
charged for handling etc. is allowed.)
:li.You may modify this program if any modification is well documented
and this documentation is added to any copy you distribute. You may
not change this part of the documentation.
:eol.
:p.
The source code of Connect Four is not free. Although this program is
meant mainly as a programming example we think that some money given as an
acknowledgement for our work is fair. If you have received the source
code from us you are responsible that not more than three persons have
access to these documents. (This means you are allowed to share the
source code with two other persons of your choice.) Under no
circumstances you are allowed to resell or distribute the source code.
(Of course you may use small parts of it for your own programs and
distribute them. In this context "small parts" means that some functions
written by us may appear somewhere in your program.) If you want to
know more about this, please refer to :link reftype=hd refid=6502 .
Getting the Sources:elink..
:p.
If you have any questions about your rights please send us mail.
:note.
We don&apos.t like all this talking about terms, conditions etc. so this
page is surely not complete. Our main "message" is&colon. Everything is 
allowed as long as the rights of anybody else are not affected and as
long as you don&apos.t reduce our chances to earn some money by selling
the sources. 
:p.
Any of the above restrictions can&apos.t be verified by us. At least we
have more interesting things to do so we appeal to your fairness and 
to your conscience.
:p.
Related topics&colon.
:sl compact.
:li.:link reftype=hd refid=6000 .How to Play Connect Four:elink.
:li.:link reftype=hd refid=6502 .Getting the Sources:elink.
:li.:link reftype=hd refid=6501 .About the Authors:elink.
:esl.

:h1 id=6504 .Plans for Future Versions
:i1.Plans for Future Versions
:p.
We are planning to add or modify the following topics:
:ul.
:li. We want to add the possibility of selecting the board size and the
number of coins that lead to victory.
:li. We want to add the choice between several valuation functions.
:li. We want to set the loudness of the sound to the value given
by the MMPM/2 volume button.
:eul.

:h1 id=6505 .The Connect Four Contest
:i1.The Connect Four Contest
:p.
When you have already read some parts of this online help, you might
have noticed that one of the difficulties in programming this game
was to develop the valuation function. This function is needed when
the computer has calculated a certain number of moves and did
not find a winning or loosing situation. A very simple method to
validate a game situation is to give it the value "0" when it did
not lead to victory or loss. But when the game is played at lower
levels, its first phase will be quite boring. In some games a good
valuation function is very important and difficult (for example Chess),
in others it is trivial (for example when you only have to count a
score). In Connect Four you have to look at a given game situation
and say whether it is "good" or "bad". 
.br
If you want to know more about the theory behind this game, please
read :link reftype=hd refid=PANEL_AI.How the Computer Finds a Move:elink..
:p.
So, it&apos.s up to you! Send us YOUR valuation function, and it will
take part in the contest! 
:p.
What the function should do&colon.
:ul.
:li.It should return a number which is the valuation for a given
game situation. You can limit this function to work only with 
the board size 7 columns and 6 rows, but you can also use a
variable board size.
:li.It must not modify the game board. This
means that it is not allowed to perform any moves and look what the 
situation is like two moves later. 
:li.It should be of reasonable speed.
This means it should not take longer than one minute to calculate a
move at level eight (when it&apos.s embedded into our Connect Four).
:li.It should be written in EMX-C and must be bug-free. 
You should of course use all the naming conventions for the variables 
used in the program.
:eul.:p.
We will make our judgement in regard to several factors&colon. First of
all, a very important fact is how good a function plays, so we&apos.ll let 
them play against one another on the levels 4, 5 and 6, as starting 
and as second player. But speed will also be
regarded, and how interesting a game is (this is naturally a quite
subjective topic). 
:p.
If you are interested in taking part in
this contest, please contact us. Our addresses are listed in 
:link reftype=hd refid=6501 .About the Authors:elink..
We will send you information about our naming conventions,
and a little text-only version of the game where you can include your 
code for testing purposes.

:p.
:hp2.The most interesting thing: The prize!:ehp2.
.br
Well, we are no commercial software company, and writing programs
is only our hobby. So please don&apos.t expect any expensive prizes.
We only do this because we are curious to see good valuation functions,
and because we want to know how good a computer can be in playing
Connect Four.
The one thing we can offer as a prize is a free version of Connect 
Four&apos.s
source code. When you already got
the source code from us, we will refund your payment.
It is highly recommended to get the source code when you want to
take part in this contest because for you it will be much easier to 
test and improve your function (and you can have a look at how we solved
this problem). On information how to do this please refer to
:link reftype=hd refid=6502 .Getting the Sources:elink..
If the functions&apos. author(s) don&apos.t refuse,
we will release a version of Connect Four with the best functions.
:p.
Date of expiry&colon.
.br
This is a difficult topic. On the one hand, many people don&apos.t
check the FTP servers too frequently, so it might take some time until
a recent number of persons has got this program. Then, they might want
to get the source code and get familiar with it. At last, they have
to develop an idea for a nice function and program it. All these
things take time.
.br
On the other hand, nobody wants to take part in a contest that will
expire in, say, five years. And we don&apos.t want to wait for five years
to check the results. So we think that a good expiry date will be
the 31.12.1994. If anybody wants to send us a valuation function after
this date, he is welcome to do so, but he will not take part any more.
Of course we will inform all competitors about the results.
:p.
We wish you much fun in developing and programming a good function!
:p.
Related Topics&colon.
:sl compact.
:li.:link reftype=hd refid=PANEL_AI.How the Computer Finds a Move:elink.
:li.:link reftype=hd refid=6000 .How to Play Connect Four:elink.
:li.:link reftype=hd refid=6502 .Getting the Sources:elink.
:li.:link reftype=hd refid=6501 .About the Authors:elink.
:esl.


:euserdoc.
