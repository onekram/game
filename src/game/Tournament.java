package game;

import game.board.Board;
import game.player.Player;

import java.util.*;

public class Tournament {
    private final List<Player> players;
    private final List<List<Player>> rounds = new ArrayList<>();
    private final boolean log;

    public Tournament(final List<Player> players, final boolean log) {
        this.players = players;
        Collections.shuffle(this.players);
        this.log = log;

    }
    private int getNear2n(final Deque<Player> playerDeque) {
        int n = playerDeque.size();
        int res = 1;
        while (res * 2 < n) {
            res *= 2;
        }
        return res;
    }

    private void round(int games, Board board, int no, final Deque<Player> playerDeque) {
        rounds.add(new ArrayList<>());
        for (int i = 0; i < games; i++) {
            Player player1 = playerDeque.pop();
            Player player2 = playerDeque.pop();
            Game game = new Game(log, player1,player2);
            Player winner = game.playTillWinner(board);
            rounds.get(no).add(player1);
            rounds.get(no).add(player2);
            playerDeque.addLast(winner);
        }
    }
    public void startTournament(Board board) {
        rounds.clear();
        final Deque<Player> playerDeque = new ArrayDeque<>(players);
        int no = 0;
        while (playerDeque.size() > 1) {
            round(playerDeque.size() - getNear2n(playerDeque), board, no, playerDeque);
            board.clear();
            ++no;
        }
        rounds.add(new ArrayList<>());
        rounds.get(no).add(playerDeque.pop());
    }

    private static void addSpaces(final StringBuilder sb, final int n) {
        sb.append(" ".repeat(n));
    }
    public void showTable() {
        if (rounds.isEmpty()) {
            System.err.println("Error: No tournaments happened\nCan't show table");
            return;
        }
        StringBuilder sb = new StringBuilder();
        sb.append("Tournament table\n");
        int lineLen = (int) Math.pow(2, rounds.size() + 2);
        int c = lineLen;
        for (int i = rounds.size() - 1; i >= 1; --i) {
            List<Player> r = rounds.get(i);
            sb.append(rounds.size() - i);
            sb.append('.');
            boolean first = true;
            for (Player player : r) {
                if (first) {
                    addSpaces(sb, c / 2 - player.toString().length());
                    first = false;
                } else {
                    addSpaces(sb, c  - player.toString().length());
                }
                sb.append(player);
            }
            c /= 2;
            sb.append('\n');
        }
        List<Player> last = rounds.get(0);
        if (rounds.size() >= 2 && rounds.get(1).size() * 2 > last.size()) {
            StringBuilder esb = new StringBuilder();
            for (Player player : last) {
                esb.append(player);
                addSpaces(esb, c - player.toString().length());
            }
            sb.append(rounds.size());
            sb.append('.');
            addSpaces(sb, lineLen - esb.length());
            sb.append(esb);
            sb.append('\n');
        }
        System.out.print(sb);
    }
}
