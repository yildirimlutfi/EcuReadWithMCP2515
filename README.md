Sistemde 3 MCU bulunmaktadır. Bunlarından ikisi Atmega 328P(Uno) diğeri PIC24FJ256 (Stone) işlemcileridir. 1. Atmega ECU diğer Atmega Can request olarak çalıştırılmış ve Can request olan Atmega ECU’dan aldığı verileri PIC’e I2C üzerinden iletmiştir. I2C master PIC, slave ise Atmega işlemci olmuştur. 

Arduino işlemcilerinin örnek yazılımları Arduino IDE yazılımı içerisinden Tools>Manage Libraries>mcp_can yazılarak indirilebilir. İndirme işlemi bittikten sonra örnek yazılımlar File>Examples>mcp_can içerisinden bulunabilir. PIC yazılımı ise yeni proje oluşturulup dosya içerisindeki kodlar kopyalanarak oluşturulabilir. 

Sistemi doğrulamak için CAN hattına ELM cihazı bağlanarak test edilmiştir. Daha sonra aynı cihaz ile Passat ECU’ya bağlanarak da izleme yapılmıştır. Bu durumda kurulan sistem ile gerçek ECU sistemi simülasyonu yapıldığı doğrulanmıştır. Burada sadece ECU’dan veriler okunmuştur herhangi bir değer ECU’ya yazılmamıştır. 

![CanbusDiagram](https://github.com/yildirimlutfi/EcuReadWithMCP2515/assets/58117960/b214239f-59fe-4f35-9ca1-8bdfc0134db9)
