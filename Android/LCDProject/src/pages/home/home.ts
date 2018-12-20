import { Component, NgZone } from '@angular/core';
import { NavController, ToastController } from 'ionic-angular';
import { BLE } from '@ionic-native/ble';
import { DetailPage } from '../detail/detail';

@Component({
  selector: 'page-home',
  templateUrl: 'home.html'
})
export class HomePage {
  devices: any[] = [];
  statusMessage: string;

  constructor(public navCtrl: NavController,
    private toastCtrl: ToastController,
    private ble: BLE,
    private ngZone: NgZone) {
  }

  ionViewDidEnter() {
    console.log('ionViewDidEnter');
    this.scan;
  }

  scan() {
    this.setStatus('Scanning for Bluetooth LE Devices');
    this.devices = [];
    this.ble.scan([], 5).subscribe(
      device => this.onDeviceDiscovered(device),
      error => this.scanError(error)
    );
    setTimeout(this.setStatus.bind(this), 5000, 'Scan complete');
  }
  onDeviceDiscovered(device) {
    console.log('Discovered ' + JSON.stringify(device, null, 2));
    this.ngZone.run(() => {
      this.devices.push(device);
    });
  }

  scanError(error) {
    this.setStatus('Error' + error);
    let toast = this.toastCtrl.create({
      message: 'Error scanning for Bluetooth low energy devices',
      position: 'middle',
      duration: 5000
    });
    toast.present();
  }

  deviceSelected(device) {
    console.log(JSON.stringify(device) + 'Selected');
    this.navCtrl.push(DetailPage, {
      device: device
    });
  }

  setStatus(message) {
    console.log(message);
    this.ngZone.run(() => {
      this.statusMessage = message;
    });
  }
}
