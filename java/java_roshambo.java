import java.util.Random;
import java.util.Scanner;

public class RockPaperScissors {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        Random random = new Random();

        String[] options = {"가위", "바위", "보"};

        while (true) {
            System.out.print("가위(0), 바위(1), 보(2) 중 하나를 선택하세요. (종료: -1): ");
            int userChoice = scanner.nextInt();

            if (userChoice == -1) {
                System.out.println("게임을 종료합니다.");
                break;
            }

            if (userChoice < 0 || userChoice > 2) {
                System.out.println("올바른 숫자를 입력하세요.");
                continue;
            }

            int computerChoice = random.nextInt(3);

            System.out.println("사용자: " + options[userChoice]);
            System.out.println("컴퓨터: " + options[computerChoice]);

            if (userChoice == computerChoice) {
                System.out.println("무승부!");
            } else if ((userChoice == 0 && computerChoice == 2) ||
                       (userChoice == 1 && computerChoice == 0) ||
                       (userChoice == 2 && computerChoice == 1)) {
                System.out.println("사용자 승리!");
            } else {
                System.out.println("컴퓨터 승리!");
            }
        }

        scanner.close();
    }
}
