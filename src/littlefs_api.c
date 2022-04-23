/**
 * @file littlefs_api.c
 * @brief Maps the HAL of esp_partition <-> littlefs
 * @author Brian Pugh
 */

#define ESP_LOCAL_LOG_LEVEL ESP_LOG_INFO

#include "esp_log.h"
//#include "esp_partition.h"
#include "sdmmc_cmd.h"
#include "esp_vfs.h"
#include "littlefs/lfs.h"
#include "esp_littlefs.h"
#include "littlefs_api.h"

static const char TAG[] = "esp_littlefs_api";

extern sdmmc_card_t stSDCard;

int littlefs_api_read(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, void *buffer, lfs_size_t size) {
    esp_littlefs_t * efs = c->context;
    //ESP_LOGI(TAG, "read block %08x, offset %08x, size %08x", block, off, size);
    size_t part_off = (block * c->block_size) + off;
    //esp_err_t err = esp_partition_read(efs->partition, part_off, buffer, size);
    esp_err_t err = sdmmc_read_sectors(&stSDCard, buffer, part_off / stSDCard.csd.sector_size, size / stSDCard.csd.sector_size);
    if (err) {
        ESP_LOGE(TAG, "failed to read addr %08x, size %08x, err %d", part_off, size, err);
        return LFS_ERR_IO;
    }
    return 0;
}

int littlefs_api_prog(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size) {
    esp_littlefs_t * efs = c->context;
    //ESP_LOGI(TAG, "write block %08x, offset %08x, size %08x", block, off, size);
    size_t part_off = (block * c->block_size) + off;
    //esp_err_t err = esp_partition_write(efs->partition, part_off, buffer, size);
    esp_err_t err = sdmmc_write_sectors(&stSDCard, buffer, part_off / stSDCard.csd.sector_size, size / stSDCard.csd.sector_size);
    if (err) {
        ESP_LOGE(TAG, "failed to write addr %08x, size %08x, err %d", part_off, size, err);
        return LFS_ERR_IO;
    }
    return 0;
}

int littlefs_api_erase(const struct lfs_config *c, lfs_block_t block) {
    esp_littlefs_t * efs = c->context;
    size_t part_off = block * c->block_size;
    //esp_err_t err = esp_partition_erase_range(efs->partition, part_off, c->block_size);
    esp_err_t err = ESP_OK;
    if (err) {
        ESP_LOGE(TAG, "failed to erase addr %08x, size %08x, err %d", part_off, c->block_size, err);
        return LFS_ERR_IO;
    }
    return 0;

}

int littlefs_api_sync(const struct lfs_config *c) {
    /* Unnecessary for esp-idf */
    return 0;
}

