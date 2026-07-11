CC ?= gcc
CFLAGS ?= -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-builtin -nostdlib -O2 -Wall -Wextra -I./include -Idoomgeneric
AS ?= nasm
LDFLAGS ?= -m32 -nostdlib -static -T linker.ld -no-pie
LIBGCC := $(shell $(CC) -m32 -print-libgcc-file-name)

all: build/holokernel.bin build/holokernel.iso

build:
	mkdir -p build

DOOM_OBJ_NAMES = dummy.o am_map.o doomdef.o doomstat.o dstrings.o d_event.o d_items.o d_iwad.o d_loop.o d_main.o d_mode.o d_net.o f_finale.o f_wipe.o g_game.o hu_lib.o hu_stuff.o info.o i_cdmus.o i_endoom.o i_joystick.o i_scale.o i_sound.o i_system.o i_timer.o memio.o m_argv.o m_bbox.o m_cheat.o m_config.o m_controls.o m_fixed.o m_menu.o m_misc.o m_random.o p_ceilng.o p_doors.o p_enemy.o p_floor.o p_inter.o p_lights.o p_map.o p_maputl.o p_mobj.o p_plats.o p_pspr.o p_saveg.o p_setup.o p_sight.o p_spec.o p_switch.o p_telept.o p_tick.o p_user.o r_bsp.o r_data.o r_draw.o r_main.o r_plane.o r_segs.o r_sky.o r_things.o sha1.o sounds.o statdump.o st_lib.o st_stuff.o s_sound.o tables.o v_video.o wi_stuff.o w_checksum.o w_file.o w_main.o w_wad.o z_zone.o w_file_stdc.o i_input.o i_video.o doomgeneric.o

DOOM_OBJS = $(addprefix doomgeneric/, $(DOOM_OBJ_NAMES))
KERNEL_OBJS = boot.o kernel.o interrupts.o gdt.o gdt_flush.o pmm.o vmm.o kheap.o libc.o doomgeneric_holokernel.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

build/holokernel.bin: $(KERNEL_OBJS) $(DOOM_OBJS) | build
	$(CC) $(LDFLAGS) -o $@ $^

DOOM1.WAD:
	curl -k -L -o DOOM1.WAD "https://distro.ibiblio.org/slitaz/sources/packages/d/doom1.wad"

build/holokernel.iso: build/holokernel.bin grub.cfg DOOM1.WAD
	mkdir -p build/iso/boot/grub
	cp build/holokernel.bin build/iso/boot/holokernel.bin
	cp DOOM1.WAD build/iso/boot/DOOM1.WAD
	cp grub.cfg build/iso/boot/grub/grub.cfg
	grub-mkrescue -o build/holokernel.iso build/iso > /dev/null 2>&1

run: build/holokernel.iso
	qemu-system-x86_64 -cdrom build/holokernel.iso -serial stdio

clean:
	rm -rf build

.PHONY: all run clean
