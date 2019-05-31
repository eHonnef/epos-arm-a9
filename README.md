# Portando o epos para o cortex A9

## Arquivos/links úteis

[TCC do cara que portou o epos pra a9](https://repositorio.ufsc.br/handle/123456789/184252)  
[Documentação da RealView A9](http://infocenter.arm.com/help/topic/com.arm.doc.dui0440b/index.html)  
[Modelo/máquina Zync.h](https://epos.lisha.ufsc.br/svn/epos2/trunk/include/machine/cortex/zynq.h)  
[Modelo/máquina Emote3.h](https://epos.lisha.ufsc.br/svn/epos2/trunk/include/machine/cortex/emote3.h)  

Se ver as outras máquina da pra perceber que é possível remover alguns dos registradores e adicionar outros.

## TODO list

Em ordem de prioridade:

- [x] Fazer o escalonador funcionar.
- [ ] Verificar porque a LM3S811 dá erro de `out of memory`. Porque acho que vai acontecer também com a realview se não resolver nela primeiro.
- [x] Mapear a memória da realview no respectivo arquivo `include/cortex_a/realview`. [Link referência](http://infocenter.arm.com/help/topic/com.arm.doc.dui0440b/index.html).
  - Alguns endereços não existem / não encontrei
- Implementar de acordo com realview os seguintes métodos em `include/cortex_a/realview_pbx.h`:
  - [ ] reboot
  - [ ] delay
  - [ ] enable_uart
  - [ ] power_uart
  - [ ] power_user_timer
  - [ ] power_usb
  - [ ] gpio_init
  - [ ] power_gpio
  - [ ] gpio_pull_up *
  - [ ] gpio_pull_down *
  - [ ] gpio_floating *
  - [ ] scr *
  - [ ] systick *
  - [ ] tsc *
  - [ ] timer0 *
  - [ ] timer1 *
  - [ ] gpioa *
  - [ ] gpiob *
  - [ ] gpioc *
  - [ ] gpiod *
  - [ ] gpio *
- \* "basta" fazer deslocamento de bits. (a.k.a. escovamento de bits).
- [ ] Implementar o Timer_init
  - Seguir a base do cortex-m
- [ ] Interrupções:
  - [ ] Setar todas as interrupções para `int_not()`, usando um laço de 0-64 em IC::init().
  - [ ] Desabilitar as interrupções da CPU, chamando CLI (a.k.a. clear interruption flag).
  - [ ] (?) Implementar a Engine::init()
  - [ ] (?) Implementar a Engine::disable()
  - [ ] (?) Implementar a Engine::enable()
  - [ ] IC::int_vector
  - [ ] IC::enable()

## Algumas observações talvez importantes

- WFI (wait for interrupt) é o equivalente ao HALT do x86.
- `pc_setup.cc` linhas 420-427, aparentemente reserva espaço de memória para as entidades que serão utilizadas durante o setup.
- `src/init/init_system.cc` esse arquivo é importante... por algum motivo.
- O erro de `out of memory` acontece na função `alloc` de heap.h, aparentemente ele não ta encontrando um bloco de memória livre.