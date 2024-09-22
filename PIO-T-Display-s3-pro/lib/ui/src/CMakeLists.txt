SET(SOURCES screens/ui_loading.c
    screens/ui_wifiProv.c
    screens/ui_crypto.c
    screens/ui_chart.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_images_logo_png.c
    images/ui_img_1283391855.c
    images/ui_img_213572400.c
    images/ui_img_611106281.c
    images/ui_img_images_btc_png.c
    images/ui_img_images_bg_png.c
    images/ui_img_537082870.c
    fonts/ui_font_Font100.c
    fonts/ui_font_Font25.c
    fonts/ui_font_Font38.c)

add_library(ui ${SOURCES})
