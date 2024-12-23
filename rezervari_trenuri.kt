import java.time.LocalDate
import java.time.format.DateTimeFormatter
import java.time.temporal.ChronoUnit
import java.io.*
import java.util.*
import java.io.File
import java.io.FileNotFoundException
import java.lang.Exception

// Functie pentru extinderea cheii
fun extendKey(text: String, key: String): String {
    val extendedKey = StringBuilder()
    var keyIndex = 0
    for (i in text.indices) {
        extendedKey.append(key[keyIndex])
        keyIndex = (keyIndex + 1) % key.length
    }
    return extendedKey.toString()
}

// Functie pentru criptarea textului folosind algoritmul Vigenère Cipher
fun encryptVigenere(text: String, key: String): String {
    val extendedKey = extendKey(text, key)
    val cipherText = StringBuilder()

    for (i in text.indices) {
        val char = text[i]
        if (char.isLetter()) {
            val base = if (char.isUpperCase()) 'A' else 'a'
            val textChar = char - base
            val keyChar = extendedKey[i].lowercaseChar() - 'a'
            val encryptedChar = ((textChar + keyChar) % 26 + base.code).toChar()
            cipherText.append(encryptedChar)
        } else {
            cipherText.append(char) // Caracterul rămâne nemodificat dacă nu este literă
        }
    }
    return cipherText.toString()
}

// Clasa pentru Cursa
class Cursa(
    var destinatie: String = "",
    var plecare: String = "",
    var data: String = "",
    var ora: String = "",
    var clasa: Int = 0,
    var numarLocuri: Int = 0
) {
    // Setter-e și getter-e implicite în Kotlin prin utilizarea proprietăților

    // Verificăm dacă data este în trecut
    fun isPastDate(data: String): Boolean {
        val formatter = DateTimeFormatter.ofPattern("dd/MM/yyyy")
        val parsedDate = LocalDate.parse(data, formatter)
        val currentDate = LocalDate.now()
        return ChronoUnit.DAYS.between(parsedDate, currentDate) > 0
    }

    // Verificăm dacă numele orașului este valid
    fun isCityNameValid(city: String): Boolean {
        return city.all { it.isLetter() || it.isWhitespace() }
    }

    // Rezervare locuri
    fun rezervaLocuri(nrLocuri: Int): Boolean {
        return if (numarLocuri >= nrLocuri) {
            numarLocuri -= nrLocuri
            true // Rezervare reușită
        } else {
            false // Nu există suficiente locuri
        }
    }

    // Returnează detaliile cursei
    fun getDetails(): String {
        return "$destinatie, $plecare, $data, $ora, Clasa: $clasa, Locuri: $numarLocuri"
    }
}


class Operator(private val username: String, private val password: String) {

    // Login pentru operatori
    fun login(user: String, pass: String) {
        val file = File("operator.csv")

        if (!file.exists()) {
            throw RuntimeException("Fisierul operator.csv nu poate fi deschis")
        } else {
            println("Fisierul a fost deschis cu succes")
        }

        var found = false
        file.forEachLine { line ->
            val tokens = line.split(",")
            if (tokens.size == 2) {
                val fileUser = tokens[0].trim()
                val filePass = tokens[1].trim()

                // Debugging: afișează valorile citite
                println("Din fisier: username = '$fileUser', password = '$filePass'")

                if (fileUser == user && filePass == pass) {
                    found = true
                    return@forEachLine
                }
            }
        }

        if (!found) {
            throw IllegalArgumentException("LOGIN NEREUSIT!! Username sau parola gresita!")
        }
    }

    // Adăugare cursă
    fun adaugaCursa(cursa: Cursa) {
        if (cursa.destinatie.isEmpty() || cursa.plecare.isEmpty() || 
            cursa.data.isEmpty() || cursa.ora.isEmpty() || 
            cursa.clasa == 0 || cursa.numarLocuri == 0) {
            throw IllegalArgumentException("Nu ati completat toate datele despre cursa!")
        }

        // Validare pentru formatul datei
        if (!Regex("""\d{2}/\d{2}/\d{4}""").matches(cursa.data)) {
            throw IllegalArgumentException("Format data gresit! Incercati sa introduceti data sub forma: DD/MM/YYYY")
        }

        // Verificare dacă numele orașului este valid
        if (!cursa.isCityNameValid(cursa.destinatie) || !cursa.isCityNameValid(cursa.plecare)) {
            throw IllegalArgumentException("Orasul contine caractere nevalide, folositi doar litere si spatii!")
        } 

        // Verificare dacă data este în trecut
        if (cursa.isPastDate(cursa.data)) {
            throw IllegalArgumentException("Data pe care ati introdus-o este din trecut!")
        }

        val outFile = File("curse.csv").apply {
            if (!exists()) createNewFile()
        }

        outFile.appendText(
            "${cursa.destinatie},${cursa.plecare},${cursa.data},${cursa.ora},${cursa.clasa},${cursa.numarLocuri}\n"
        )
        println("Cursa a fost adaugata cu succes!!")
    }

    // Ștergerea unei curse
    fun stergeCursa(destinatie: String, plecare: String, data: String, ora: String) {
        val inFile = File("curse.csv")
        if (!inFile.exists()) {
            throw RuntimeException("Nu s-a putut deschide fisierul pentru citire.")
        }

        val tempFile = File("temp.csv")
        if (tempFile.exists()) tempFile.delete()

        var found = false

        inFile.forEachLine { line ->
            val tokens = line.split(",")
            if (tokens.size >= 4) {
                val fileDestinatie = tokens[0].trim()
                val filePlecare = tokens[1].trim()
                val fileData = tokens[2].trim()
                val fileOra = tokens[3].trim()

                if (fileDestinatie == destinatie && filePlecare == plecare && fileData == data && fileOra == ora) {
                    found = true // cursa a fost găsită
                } else {
                    tempFile.appendText(line + "\n") // păstrăm linia în fișierul temporar
                }
            }
        }

        if (!found) {
            tempFile.delete()
            throw IllegalArgumentException("Cursa specificata nu a fost gasita")
        }

        if (!inFile.delete() || !tempFile.renameTo(inFile)) {
            throw RuntimeException("Eroare la actualizarea fisierului curse.csv")
        }

        println("Cursa a fost stearsa cu succes!")
    }
}

class Utilizator(private val username: String, private val email: String, private val password: String) {

    // Email validation
    fun isValidEmail(email: String): Boolean {
        if (!email.first().isLetter()) {
            throw IllegalArgumentException("Email invalid: primul caracter nu este o litera!")
        }

        val atPos = email.indexOf('@')
        val dotPos = email.indexOf('.')

        if (atPos == -1) throw IllegalArgumentException("Email invalid: lipseste caracterul @.")
        if (dotPos == -1) throw IllegalArgumentException("Email invalid: lipseste caracterul .")
        if (atPos > dotPos) throw IllegalArgumentException("Email invalid: caracterul . apare înainte de @.")
        if (dotPos == email.lastIndex) throw IllegalArgumentException("Email invalid: '.' nu poate fi ultimul caracter.")
        
        return true
    }

    // Password strength validation
    fun passwordStrength(password: String): String {
        val hasLower = password.any { it.isLowerCase() }
        val hasUpper = password.any { it.isUpperCase() }
        val hasDigit = password.any { it.isDigit() }
        val hasSpecial = password.any { !it.isLetterOrDigit() }

        if (password.length < 8) {
            throw IllegalArgumentException("Password is too short. Minimum length is 8 characters.")
        }

        val strength = listOf(hasLower, hasUpper, hasDigit, hasSpecial).count { it }

        return when (strength) {
            in 0..1 -> throw IllegalArgumentException("Password is weak.")
            2 -> "ok"
            else -> "good"
        }
    }

    // Check for duplicate username or email
    fun isDuplicate(username: String, email: String): Boolean {
        val file = File("utilizator.csv")
        if (!file.exists()) return false

        return file.useLines { lines ->
            lines.any { line ->
                val (existingUsername, existingEmail, _) = line.split(",")
                existingUsername == username || existingEmail == email
            }
        }
    }

    // Register a user
    fun autentificare(username: String, email: String, parola: String) {
        isValidEmail(email)

        println("Va rugam sa introduceti din nou aceeasi parola pentru confirmare: ")
        val confirmare = readLine() ?: ""
        if (confirmare != parola) {
            throw IllegalArgumentException("Parolele nu corespund!")
        }

        val encryptedPassword = encryptVigenere(parola, "key")

        File("utilizator.csv").appendText("$username,$email,$encryptedPassword\n")
        println("Autentificare reusita!")
    }

    // User login
    fun login(user: String, pass: String) {
        val file = File("utilizator.csv")
        if (!file.exists()) {
            throw FileNotFoundException("Fisierul utilizator.csv nu exista!")
        }

        val encryptedPassword = encryptVigenere(pass, "key")
        val found = file.useLines { lines ->
            lines.any { line ->
                val (fileUser, _, filePass) = line.split(",")
                fileUser == user && filePass == encryptedPassword
            }
        }

        if (!found) {
            throw IllegalArgumentException("LOGIN NEREUSIT! Username sau parola gresita!")
        }

        println("Login reusit!")
    }

    // Reserve seats for a train
    fun rezervareLoc(
        username: String, destinatie: String, plecare: String, data: String,
        ora: String, clasa: Int, nrlocuri: Int
    ) {
        val file = File("curse.csv")
        if (!file.exists()) {
            throw FileNotFoundException("Fisierul curse.csv nu exista!")
        }

        val tempFile = File("curse_temp.csv")
        var found = false

        file.useLines { lines ->
            tempFile.bufferedWriter().use { writer ->
                lines.forEach { line ->
                    val parts = line.split(",")
                    if (parts.size != 6) {
                    throw IllegalArgumentException("Linia din fisier nu contine exact 6 campuri: $line")
                    }

                    val fileDestinatie = parts[0]
                    val filePlecare = parts[1]
                    val fileData = parts[2]
                    val fileOra = parts[3]
                    val fileClasa = parts[4].toIntOrNull() ?: throw IllegalArgumentException("Clasa trebuie sa fie un numar!")
                    var fileLocuri = parts[5].toIntOrNull() ?: throw IllegalArgumentException("Numarul de locuri trebuie să fie un numar!")


                    if (fileDestinatie == destinatie && filePlecare == plecare &&
                        fileData == data && fileOra == ora && fileClasa == clasa) {
                        found = true
                        if (fileLocuri >= nrlocuri) {
                            fileLocuri -= nrlocuri
                            println("Locuri rezervate cu succes! Locuri ramase: $fileLocuri")
                        } else {
                            throw IllegalArgumentException("Nu sunt suficiente locuri disponibile!")
                        }
                    }
                    writer.write("$fileDestinatie,$filePlecare,$fileData,$fileOra,$fileClasa,$fileLocuri\n")
                }
            }
        }

        if (!found) {
            throw IllegalArgumentException("Cursa specificata nu exista!")
        }

        file.delete()
        tempFile.renameTo(file)

        File("rezervari.csv").appendText("$username,$destinatie,$plecare,$data,$ora,$clasa\n")
    }

    // Vigenere cipher encryption
    fun encryptVigenere(text: String, key: String): String {
        val keyLen = key.length
        return text.mapIndexed { i, char ->
            if (char.isLetter()) {
                val shift = key[i % keyLen].lowercaseChar() - 'a'
                if (char.isLowerCase()) {
                    'a' + (char - 'a' + shift) % 26
                } else {
                    'A' + (char - 'A' + shift) % 26
                }
            } else {
                char
            }
        }.joinToString("")
    }
}

// Display available courses
fun afiseazaCurseDisponibile() {
    val file = File("curse.csv")
    if (!file.exists()) {
        throw FileNotFoundException("Fisierul curse.csv nu exista!")
    }

    println("CURSELE DISPONIBILE SUNT:")
    file.forEachLine { line ->
        val parts = line.split(",")
        
        // Verificăm dacă linia conține exact 6 elemente
        if (parts.size != 6) {
            println("Linia nu contine date valide: $line")
            return@forEachLine
        }

        val destinatie = parts[0]
        val plecare = parts[1]
        val data = parts[2]
        val ora = parts[3]
        val clasa = parts[4]
        val locuri = parts[5]

        // Validăm dacă clasa și locurile sunt valori numerice
        val clasaInt = clasa.toIntOrNull()
        val locuriInt = locuri.toIntOrNull()

        if (clasaInt == null || locuriInt == null) {
            println("Linia contine date invalide pentru clasa sau numarul de locuri: $line")
            return@forEachLine
        }

        // Afișăm informațiile despre cursă
        println("$destinatie -> $plecare | Data: $data | Ora: $ora | Clasa: $clasaInt | Locuri disponibile: $locuriInt")
    }
}


fun main() {
    try {
        println("Bine ati venit!")

        while (true) {
            println("\nSelectati rolul dumneavoastra (operator sau utilizator) sau tastati 'exit' pentru a iesi: ")
            val userType = readLine()?.lowercase() ?: ""

            if (userType == "exit") {
                println("La revedere!")
                break
            }

            when (userType) {
                "operator" -> {
                    println("Introduceti username-ul operatorului: ")
                    val username = readLine() ?: ""
                    println("Introduceti parola operatorului: ")
                    val password = readLine() ?: ""

                    val operator = Operator(username, password)

                    try {
                        operator.login(username, password)
                        println("Login reusit pentru operator!")

                        var optiune: Int
                        do {
                            println("\nMeniu operator:")
                            println("1. Adauga cursa")
                            println("2. Sterge cursa")
                            println("3. Logout")
                            print("Selectati o optiune: ")
                            optiune = readLine()?.toIntOrNull() ?: 0

                            when (optiune) {
                                1 -> {
                                    val cursa = Cursa()

                                    print("Introduceti destinatia: ")
                                    val destinatie = readLine() ?: ""
                                    cursa.destinatie = destinatie

                                    print("Introduceti locul de plecare: ")
                                    val plecare = readLine() ?: ""
                                    cursa.plecare = plecare

                                    print("Introduceti data (DD/MM/YYYY): ")
                                    val data = readLine() ?: ""
                                    cursa.data = data

                                    print("Introduceti ora (HH:MM): ")
                                    val ora = readLine() ?: ""
                                    cursa.ora = ora

                                    print("Introduceti clasa trenului (1 sau 2): ")
                                    val clasa = readLine()?.toIntOrNull() ?: 0
                                    cursa.clasa = clasa

                                    print("Introduceti numarul de locuri: ")
                                    val locuri = readLine()?.toIntOrNull() ?: 0
                                    cursa.numarLocuri = locuri

                                    operator.adaugaCursa(cursa)
                                }

                                2 -> {
                                    print("Introduceti destinatia cursei de sters: ")
                                    val destinatie = readLine() ?: ""

                                    print("Introduceti locul de plecare: ")
                                    val plecare = readLine() ?: ""

                                    print("Introduceti data cursei de sters (DD/MM/YYYY): ")
                                    val data = readLine() ?: ""

                                    print("Introduceti ora cursei de sters (HH:MM): ")
                                    val ora = readLine() ?: ""

                                    operator.stergeCursa(destinatie, plecare, data, ora)
                                }
                            }
                        } while (optiune != 3)
                    } catch (e: Exception) {
                        println("Eroare: ${e.message}")
                    }
                }

                "utilizator" -> {
                    println("1. Autentificare utilizator nou")
                    println("2. Login utilizator existent")
                    val optiuneUtilizator = readLine()?.toIntOrNull() ?: 0

                    print("Introduceti username-ul: ")
                    val username = readLine() ?: ""

                    val utilizator = Utilizator(username, "", "")

                    when (optiuneUtilizator) {
                        1 -> {
                            print("Introduceti email: ")
                            var email = readLine() ?: ""

                            try {
                                while (utilizator.isDuplicate(username, email)) {
                                    println("Username-ul sau emailul sunt deja folosite. Introduceti altele:")
                                    print("Nume utilizator: ")
                                    email = readLine() ?: ""
                                    utilizator.isValidEmail(email)
                                }
                            } catch (e: Exception) {
                                println("Eroare: ${e.message}")
                            }

                            var passwordSet = false
                            var password: String
                            while (!passwordSet) {
                                print("Introduceti parola: ")
                                password = readLine() ?: ""

                                try {
                                    val strength = utilizator.passwordStrength(password)
                                    println("Password strength: $strength")
                                    passwordSet = true
                                    utilizator.autentificare(username, email, password)
                                } catch (e: Exception) {
                                    println("Eroare: ${e.message}")
                                }
                            }
                        }

                        2 -> {
                            try {
                                print("Introduceti parola: ")
                                val password = readLine() ?: ""
                                utilizator.login(username, password)
                                println("Login reusit!")

                                print("Doriti sa rezervati o calatorie? (1 pentru DA, 0 pentru NU): ")
                                val rezervareOptiune = readLine()?.toIntOrNull() ?: 0

                                if (rezervareOptiune == 1) {
                                    afiseazaCurseDisponibile()

                                    print("Introduceti destinatia: ")
                                    val destinatie = readLine() ?: ""

                                    print("Introduceti locul de plecare: ")
                                    val plecare = readLine() ?: ""

                                    print("Introduceti data (DD/MM/YYYY): ")
                                    val data = readLine() ?: ""

                                    print("Introduceti ora (HH:MM): ")
                                    val ora = readLine() ?: ""

                                    print("Introduceti clasa trenului (1 sau 2): ")
                                    val clasa = readLine()?.toIntOrNull() ?: 0

                                    print("Introduceti numarul de locuri: ")
                                    val locuri = readLine()?.toIntOrNull() ?: 0

                                    utilizator.rezervareLoc(username, destinatie, plecare, data, ora, clasa, locuri)
                                }
                            } catch (e: Exception) {
                                println("Eroare la login: ${e.message}")
                            }
                        }
                    }
                }

                else -> println("Rol invalid. Incercati din nou.")
            }
        }
    } catch (e: Exception) {
        println("Eroare: ${e.message}")
    }
}

