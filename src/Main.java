import game.Game;
import game.Tournament;
import game.board.*;
import game.player.*;

import java.util.Arrays;

public class Main {
    public static void main(String[] args) {
//        final Game game = new Game(true, new SequentialPlayer("Ibrahim"), new SequentialPlayer("Masha"));
//        Player res = game.playTillWinner(new MNKBoard(15, 10,15));
//        System.out.println("res: " + res);

        final Tournament tournament = new Tournament(
                Arrays.asList(
                        new SequentialPlayer("Mike"),
                        new SequentialPlayer("Masha"),
                        new SequentialPlayer("Oleg"),
                        new SequentialPlayer("Fedya"),
                        new SequentialPlayer("Lesya"),
                        new SequentialPlayer("Gnomik"),
                        new SequentialPlayer("Senya"),
                        new SequentialPlayer("John"),
                        new SequentialPlayer("Albert")
//                        new SequentialPlayer("Sasha"),
//                        new SequentialPlayer("Vanya"),
//                        new SequentialPlayer("Dima"),
//                        new SequentialPlayer("Pod"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Roma"),
//                        new SequentialPlayer("Kirill"),
//                        new SequentialPlayer("Liza"),
//                        new SequentialPlayer("Ed"),
//                        new SequentialPlayer("Lena")
                ),
                false);
//        tournament.startTournament(new CircleBoard(15, 2));
//        tournament.showTable();
        tournament.startTournament(new MNKBoard(15, 10,15));
        tournament.showTable();
    }
}
