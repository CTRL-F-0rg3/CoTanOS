import java.util.Stack;

public class MiniJVM {

    // Reprezentacja ramki wywołania metody wewnątrz naszego interpretera Java
    static class Frame {
        int[] localVariables;
        Stack<Integer> operandStack;
        byte[] code;
        int pc;

        public Frame(int maxLocals, int maxStack, byte[] code) {
            this.localVariables = new int[maxLocals];
            this.operandStack = new Stack<>();
            this.code = code;
            this.pc = 0;
        }
    }

    // Główna pętla wykonawcza (Interpreter Loop)
    public static void execute(byte[] bytecode, int maxLocals, int maxStack) {
        Frame frame = new Frame(maxLocals, maxStack, bytecode);
        
        System.out.println("[Java JVM] Uruchamianie wewnętrznego interpretera dla kodu o długości: " + bytecode.length);

        while (frame.pc < frame.code.length) {
            // Pobranie opkodu (konwersja ze ze znakiem na bez znaku za pomocą & 0xFF)
            int opcode = frame.code[frame.pc++] & 0xFF;

            switch (opcode) {
                case 0x00: // nop
                    break;

                // Ładowanie stałych
                case 0x03: // iconst_0
                    frame.operandStack.push(0);
                    break;
                case 0x04: // iconst_1
                    frame.operandStack.push(1);
                    break;
                case 0x05: // iconst_2
                    frame.operand_stack.push(2);
                    break;

                case 0x10: { // bipush (wepchnij bajt ze znakiem)
                    int val = frame.code[frame.pc++];
                    frame.operandStack.push(val);
                    break;
                }

                // Ładowanie zmiennych lokalnych (iload_0 do iload_3)
                case 0x1A: // iload_0
                case 0x1B: // iload_1
                case 0x1C: // iload_2
                case 0x1D: // iload_3 {
                    int idxLoad = opcode - 0x1A;
                    frame.operandStack.push(frame.localVariables[idxLoad]);
                    break;

                // Zapis zmiennych lokalnych (istore_0 do istore_3)
                case 0x3B: // istore_0
                case 0x3C: // istore_1
                case 0x3D: // istore_2
                case 0x3E: // istore_3 {
                    int idxStore = opcode - 0x3B;
                    frame.localVariables[idxStore] = frame.operandStack.pop();
                    break;

                // Arytmetyka: Dodawanie (iadd)
                case 0x60: {
                    int v2 = frame.operandStack.pop();
                    int v1 = frame.operandStack.pop();
                    frame.operandStack.push(v1 + v2);
                    break;
                }

                // Arytmetyka: Odejmowanie (isub)
                case 0x64: {
                    int v2 = frame.operandStack.pop();
                    int v1 = frame.operandStack.pop();
                    frame.operandStack.push(v1 - v2);
                    break;
                }

                // Skok warunkowy: if_icmpeq (jeśli równe)
                case 0x9F: {
                    int branchOffset = (frame.code[frame.pc] << 8) | (frame.code[frame.pc + 1] & 0xFF);
                    frame.pc += 2;
                    int v2 = frame.operandStack.pop();
                    int v1 = frame.operandStack.pop();
                    if (v1 == v2) {
                        frame.pc += branchOffset - 3; // Korekta względem bieżącego PC
                    }
                    break;
                }

                // Instrukcja bezwarunkowego skoku: goto
                case 0xA7: {
                    int branchOffset = (frame.code[frame.pc] << 8) | (frame.code[frame.pc + 1] & 0xFF);
                    frame.pc += 2;
                    frame.pc += branchOffset - 3;
                    break;
                }

                // Powrót / Koniec metody (return)
                case 0xB1:
                    System.out.println("[Java JVM] Metoda zakończona pomyślnie. Szczyt stosu: " + 
                                       (frame.operandStack.isEmpty() ? "pusty" : frame.operandStack.peek()));
                    return;

                default:
                    System.err.println("[Java JVM Error] Nieznany lub nieobsługiwany opkod: 0x" + 
                                       Integer.toHexString(opcode) at PC = " + (frame.pc - 1));
                    return;
            }
        }
    }

    // Testowy rozruch tego interpretera
    public static void main(String[] args) {
        // Przykładowy bajtkod:
        // iconst_2 (0x04)
        // istore_1 (0x3C) -> local[1] = 2
        // iconst_3 (0x05)
        // istore_2 (0x3D) -> local[2] = 3
        // iload_1  (0x1B)
        // iload_2  (0x1C)
        // iadd     (0x60) -> local[1] + local[2] (2 + 3 = 5)
        // return   (0xB1)
        byte[] testBytecode = {
            (byte) 0x04, // iconst_2
            (byte) 0x3C, // istore_1
            (byte) 0x05, // iconst_3
            (byte) 0x3D, // istore_2
            (byte) 0x1B, // iload_1
            (byte) 0x1C, // iload_2
            (byte) 0x60, // iadd
            (byte) 0xB1  // return
        };

        // Uruchamiamy nasz interpreter napisany w Javie z 3 zmiennymi lokalnymi i stosem o rozmiarze 10
        execute(testBytecode, 3, 10);
    }
}