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
                        new RandomPlayer("Mike"),
                        new RandomPlayer("Masha"),
                        new RandomPlayer("Oleg"),
                        new RandomPlayer("Fedya"),
                        new RandomPlayer("Lesya"),
                        new RandomPlayer("Gnomik"),
                        new RandomPlayer("Senya"),
                        new RandomPlayer("John"),
                        new RandomPlayer("Albert"),
                        new HumanPlayer("OneKram"),
                        new RandomPlayer("Liza"),
                        new RandomPlayer("Ed")
//                        new SequentialPlayer("Lena")
                ),
                true);
//        tournament.startTournament(new CircleBoard(15, 2));
//        tournament.showTable();
        tournament.startTournament(new CircleBoard(10,3));
        tournament.showTable();
    }
}
