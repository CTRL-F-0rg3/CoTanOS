import java.io.*;
import java.util.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MiniFramework {

    // --- 1. SYSTEM ZDARZEŃ (Event Bus dla GUI / Systemu) ---
    public static class Event {
        public String type;
        public int data1;
        public int data2;

        public Event(String type, int d1, int d2) {
            this.type = type;
            this.data1 = d1;
            this.data2 = d2;
        }
    }

    public interface EventListener {
        void onEvent(Event event);
    }

    public static class EventBus {
        private static final Map<String, List<EventListener>> listeners = new HashMap<>();

        public static void subscribe(String eventType, EventListener listener) {
            listeners.computeIfAbsent(eventType, k -> new ArrayList<>()).add(listener);
        }

        public static void publish(Event event) {
            List<EventListener> subs = listeners.get(event.type);
            if (subs != null) {
                for (EventListener listener : subs) {
                    listener.onEvent(event);
                }
            }
        }
    }

    // --- 2. MINI KONTENER USŁUG (Service Registry) ---
    // Zamiast ciężkiego Dependency Injection z refleksją, używamy jawnego rejestru komponentów
    public static class Container {
        private static final Map<String, Object> services = new HashMap<>();

        public static void register(String name, Object service) {
            services.put(name, service);
            System.out.println("[Framework] Zarejestrowano usługę: " + name);
        }

        @SuppressWarnings("unchecked")
        public static <T> T get(String name) {
            return (T) services.get(name);
        }
    }

    // --- TESTOWE URUCHOMIENIE FRAMEWORKU ---
    public static void main(String[] args) {
        System.out.println("[Framework] Inicjalizacja Mini-Frameworku w systemie...");

        // Rejestrujemy prostą usługę systemową (np. menedżer okien lub sterownik)
        Container.register("SystemGraphics", new Object() {
            public void drawWindow(String title) {
                System.out.println("[GUI Driver] Rysuję okno systemowe: " + title);
            }
        });

        // Rejestracja nasłuchiwacza zdarzeń myszy / klawiatury
        EventBus.subscribe("MOUSE_CLICK", event -> {
            System.out.println("[EventBus] Wykryto kliknięcie myszą w pozycję X: " + event.data1 + ", Y: " + event.data2);
            
            // Pobieramy usługę graficzną i wywołujemy jej metodę
            // (W pełnej wersji nasza MiniJVM obsłuży dynamiczne wywołania metod)
        });

        // Symulacja zdarzenia systemowego (np. kliknięcie w okno wygenerowane przez Twój C++ OS)
        System.out.println("[System] Symulacja zdarzenia sprzętowego...");
        EventBus.publish(new Event("MOUSE_CLICK", 150, 300));
    }
}
public class MiniJVM {

    // --- STRUKTURY DANYCH CLASSFILE W WEWNĘTRZNEJ JVM ---
    static class ConstantPoolEntry {
        int tag;
        String utf8Value;
        int classIndex;
        int nameAndTypeIndex;
        int nameIndex;
        int descriptorIndex;
    }

    static class Method {
        String name;
        String descriptor;
        int maxStack;
        int maxLocals;
        byte[] code;
    }

    static class LoadedClass {
        String className;
        ConstantPoolEntry[] constantPool;
        Map<String, Method> methods = new HashMap<>();
    }

    // --- OBIEKTY I STERO-ŚRODOWISKO ---
    static class JVMObject {
        String className;
        Map<String, Object> fields = new HashMap<>();
    }

    static class Frame {
        Method method;
        int pc = 0;
        int[] localVariables;
        Stack<Object> operandStack = new Stack<>();

        public Frame(Method method) {
            this.method = method;
            this.localVariables = new int[method.maxLocals];
        }
    }

    // Prosty parser pliku .class napisany w Javie
    public static LoadedClass parseClass(byte[] classBytes) {
        LoadedClass cls = new LoadedClass();
        DataInputStream dis = new DataInputStream(new ByteArrayInputStream(classBytes));

        try {
            int magic = dis.readInt();
            if (magic != 0xCAFEBABE) {
                throw new RuntimeException("Błędny Magic Number!");
            }
            dis.readUnsignedShort(); // minor
            dis.readUnsignedShort(); // major

            int cpCount = dis.readUnsignedShort();
            cls.constantPool = new ConstantPoolEntry[cpCount];

            for (int i = 1; i < cpCount; i++) {
                ConstantPoolEntry entry = new ConstantPoolEntry();
                entry.tag = dis.readUnsignedByte();
                switch (entry.tag) {
                    case 1: // UTF8
                        int len = dis.readUnsignedShort();
                        byte[] utfBytes = new byte[len];
                        dis.readFully(utfBytes);
                        entry.utf8Value = new String(utfBytes, "UTF-8");
                        break;
                    case 7: // Class
                        entry.classIndex = dis.readUnsignedShort();
                        break;
                    case 9: // Fieldref
                    case 10: // Methodref
                    case 11: // InterfaceMethodref
                        entry.classIndex = dis.readUnsignedShort();
                        entry.nameAndTypeIndex = dis.readUnsignedShort();
                        break;
                    case 12: // NameAndType
                        entry.nameIndex = dis.readUnsignedShort();
                        entry.descriptorIndex = dis.readUnsignedShort();
                        break;
                    case 3: // Integer
                    case 4: // Float
                        dis.readInt();
                        break;
                    case 5: // Long
                    case 6: // Double
                        dis.readLong();
                        i++; // Zajmuje 2 sloty
                        break;
                    case 8: // String
                        dis.readUnsignedShort();
                        break;
                    case 15: // MethodHandle
                        dis.readUnsignedByte();
                        dis.readUnsignedShort();
                        break;
                    case 16: // MethodType
                        dis.readUnsignedShort();
                        break;
                    case 18: // InvokeDynamic
                        dis.readUnsignedShort();
                        dis.readUnsignedShort();
                        break;
                }
                cls.constantPool[i] = entry;
            }

            dis.readUnsignedShort(); // access flags
            int thisClassIdx = dis.readUnsignedShort();
            cls.className = getUtf8(cls, cls.constantPool[cls.constantPool[thisClassIdx].classIndex].nameIndex);
            
            dis.readUnsignedShort(); // super class
            
            // Interfaces
            int interfacesCount = dis.readUnsignedShort();
            dis.skipBytes(interfacesCount * 2);

            // Fields
            int fieldsCount = dis.readUnsignedShort();
            for (int i = 0; i < fieldsCount; i++) {
                dis.readUnsignedShort(); // flags
                dis.readUnsignedShort(); // name index
                dis.readUnsignedShort(); // desc index
                int attrCount = dis.readUnsignedShort();
                for (int j = 0; j < attrCount; j++) {
                    dis.readUnsignedShort();
                    int attrLen = dis.readInt();
                    dis.skipBytes(attrLen);
                }
            }

            // Methods
            int methodsCount = dis.readUnsignedShort();
            for (int i = 0; i < methodsCount; i++) {
                int flags = dis.readUnsignedShort();
                int nameIdx = dis.readUnsignedShort();
                int descIdx = dis.readUnsignedShort();
                String mName = getUtf8(cls, nameIdx);
                String mDesc = getUtf8(cls, descIdx);

                Method m = new Method();
                m.name = mName;
                m.descriptor = mDesc;

                int attrCount = dis.readUnsignedShort();
                for (int j = 0; j < attrCount; j++) {
                    String attrName = getUtf8(cls, dis.readUnsignedShort());
                    int attrLen = dis.readInt();
                    if (attrName.equals("Code")) {
                        m.maxStack = dis.readUnsignedShort();
                        m.maxLocals = dis.readUnsignedShort();
                        int codeLen = dis.readInt();
                        m.code = new byte[codeLen];
                        dis.readFully(m.code);

                        // Skip exception table & attributes inside code
                        int exTableLen = dis.readUnsignedShort();
                        dis.skipBytes(exTableLen * 8);
                        int codeAttrCount = dis.readUnsignedShort();
                        for (int k = 0; k < codeAttrCount; k++) {
                            dis.readUnsignedShort();
                            int cAttrLen = dis.readInt();
                            dis.skipBytes(cAttrLen);
                        }
                    } else {
                        dis.skipBytes(attrLen);
                    }
                }
                cls.methods.put(mName + mDesc, m);
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
        return cls;
    }

    private static String getUtf8(LoadedClass cls, int index) {
        if (index > 0 && index < cls.constantPool.length && cls.constantPool[index] != null) {
            return cls.constantPool[index].utf8Value;
        }
        return "";
    }

    // --- SILNIK WYKONAWCZY (INTERPRETER LOOP) ---
    public static void executeMethod(LoadedClass cls, Method method) {
        Frame frame = new Frame(method);
        System.out.println("[Java JVM] Wykonywanie metody: " + method.name + " " + method.descriptor);

        List<JVMObject> heap = new ArrayList<>();

        while (frame.pc < frame.code.length) {
            int opcode = frame.code[frame.pc++] & 0xFF;

            switch (opcode) {
                case 0x00: // nop
                    break;
                case 0x03: // iconst_0
                    frame.operandStack.push(0);
                    break;
                case 0x04: // iconst_1
                    frame.operandStack.push(1);
                    break;
                case 0x10: { // bipush
                    int val = frame.code[frame.pc++];
                    frame.operandStack.push(val);
                    break;
                }
                case 0x1A: case 0x1B: case 0x1C: case 0x1D: { // iload_0 - 3
                    int idx = opcode - 0x1A;
                    frame.operandStack.push(frame.localVariables[idx]);
                    break;
                }
                case 0x3B: case 0x3C: case 0x3D: case 0x3E: { // istore_0 - 3
                    int idx = opcode - 0x3B;
                    frame.localVariables[idx] = (Integer) frame.operandStack.pop();
                    break;
                }
                case 0x60: { // iadd
                    int v2 = (Integer) frame.operandStack.pop();
                    int v1 = (Integer) frame.operandStack.pop();
                    frame.operandStack.push(v1 + v2);
                    break;
                }
                case 0x59: { // dup (duplikacja wartości na szczycie stosu - kluczowe dla Kotlin/Java new)
                    Object top = frame.operandStack.peek();
                    frame.operandStack.push(top);
                    break;
                }
                case 0xBB: { // new (tworzenie obiektu na podstawie Constant Pool)
                    int cpIdx = ((frame.code[frame.pc] & 0xFF) << 8) | (frame.code[frame.pc + 1] & 0xFF);
                    frame.pc += 2;
                    
                    int classNameIdx = cls.constantPool[cpIdx].classIndex;
                    String targetClass = getUtf8(cls, cls.constantPool[classNameIdx].nameIndex);
                    
                    JVMObject obj = new JVMObject();
                    obj.className = targetClass;
                    heap.add(obj);
                    
                    frame.operandStack.push(obj); // Wrzucamy referencję do obiektu na stos
                    break;
                }
                case 0xB7: { // invokespecial (wywołanie konstruktora lub metody prywatnej)
                    int cpIdx = ((frame.code[frame.pc] & 0xFF) << 8) | (frame.code[frame.pc + 1] & 0xFF);
                    frame.pc += 2;
                    // Zdejmujemy obiekt ze stosu (this dla konstruktora)
                    Object obj = frame.operandStack.pop();
                    // W tej uproszczonej wersji pomijamy faktyczne ciałko konstruktora domyślnego
                    break;
                }
                case 0xB1: // return
                    System.out.println("[Java JVM] Metoda zakończona.");
                    return;

                default:
                    System.err.println("[Java JVM Error] Nieobsługiwany opkod: 0x" + Integer.toHexString(opcode) + " przy PC=" + (frame.pc - 1));
                    return;
            }
        }
    }
}