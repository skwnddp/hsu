// 부모 클래스
class Parent {
    private String parentField;

    // 부모 클래스의 생성자
    public Parent(String parentField) {
        this.parentField = parentField;
    }

    public void parentMethod() {
        System.out.println("부모 클래스의 메서드 실행: " + parentField);
    }
}

// 자식 클래스
class Child extends Parent {
    private String childField;

    // 자식 클래스의 생성자
    public Child(String parentField, String childField) {
        // 부모 클래스의 생성자 호출
        super(parentField);
        this.childField = childField;
    }

    public void childMethod() {
        System.out.println("자식 클래스의 메서드 실행: " + childField);
    }
}

public class Main {
    public static void main(String[] args) {
        // 자식 클래스의 인스턴스 생성
        Child childInstance = new Child("부모 필드 값", "자식 필드 값");

        // 부모 클래스의 메서드 호출
        childInstance.parentMethod();

        // 자식 클래스의 메서드 호출
        childInstance.childMethod();
    }
}
