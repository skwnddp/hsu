// 부모 클래스
class Animal {
    public void makeSound() {
        System.out.println("Some generic sound");
    }
}

// 자식 클래스
class Dog extends Animal {
    @Override
    public void makeSound() {
        System.out.println("Woof! Woof!");
    }

    public void fetch() {
        System.out.println("Fetching the ball");
    }
}

public class CastingExample {
    public static void main(String[] args) {
        // 업캐스팅
        Animal animal = new Dog();
        animal.makeSound(); // 다형성에 의해 Dog 클래스의 오버라이딩된 메서드가 호출됨

        // 다운캐스팅
        if (animal instanceof Dog) {
            Dog dog = (Dog) animal;
            dog.makeSound(); // Dog 클래스의 메서드 호출
            dog.fetch();     // Dog 클래스의 추가 메서드 호출
        }
    }
}
