package game;

import java.util.Scanner;

public final class StepValueScanner {
    Scanner in;
    public StepValueScanner(Scanner in) {
        this.in = in;
    }

    public int nextInt() {
        if (in.hasNextInt()) {
            int value = in.nextInt();
           if (checkCorrect(value)) {
               return value;
           }
        } else {
            in.next();
        }
        return -1;
    }

    static boolean checkCorrect(int value) {
        return value >= 1;
    }
}
