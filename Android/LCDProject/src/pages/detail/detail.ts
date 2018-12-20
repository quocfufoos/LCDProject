import { Component, NgZone } from '@angular/core';
import { NavController, NavParams, AlertController } from 'ionic-angular';
import { BLE } from '@ionic-native/ble';

@Component({
  selector: 'page-detail',
  templateUrl: 'detail.html',
})
export class DetailPage {

  peripheral: any = {};
  power1: boolean;
  power2: boolean;
  DC1: number;
  DC2: number;
  statusMessage: string;

  constructor(public navCtrl: NavController,
    public navParams: NavParams,
    private ble: BLE,
    private alertCtrl: AlertController,
    private ngZone: NgZone) {

    let device = navParams.get('device');

    this.setStatus('Connecting to ' + device.name || device.id);

    this.ble.connect(device.id).subscribe(
      peripheral => this.onConnected(peripheral),
      peripheral => this.showAlert('Disconnected', 'The peripheral unexpectedly disconnected')
    );

  }

  onConnected(peripheral) {

    this.peripheral = peripheral;
    this.setStatus('Connected to ' + (peripheral.name || peripheral.id));

    // Update the UI with the current state of the switch characteristic
    this.ble.read(this.peripheral.id, this.peripheral.characteristics[0].service, this.peripheral.characteristics[0].characteristic).then(
      buffer => {
        let data = new Uint8Array(buffer);
        console.log('switch characteristic ' + data[0]);
        this.ngZone.run(() => {
          this.power1 = data[0] !== 0;
        });
      }
    )

    this.ble.read(this.peripheral.id, this.peripheral.characteristics[1].service, this.peripheral.characteristics[1].characteristic).then(
      buffer => {
        let data = new Uint8Array(buffer);
        console.log('switch characteristic ' + data[0]);
        this.ngZone.run(() => {
          this.power2 = data[0] !== 0;
        });
      }
    )

    // Update the UI with the current state of the dimmer characteristic
    this.ble.read(this.peripheral.id, this.peripheral.characteristics[2].service, this.peripheral.characteristics[2].characteristic).then(
      buffer => {
        let data = new Uint8Array(buffer);
        console.log('dimmer characteristic ' + data[0]);
        this.ngZone.run(() => {
          this.DC1 = data[0];
        });
      }
    )

    this.ble.read(this.peripheral.id, this.peripheral.characteristics[3].service, this.peripheral.characteristics[3].characteristic).then(
      buffer => {
        let data = new Uint8Array(buffer);
        console.log('dimmer characteristic ' + data[0]);
        this.ngZone.run(() => {
          this.DC2 = data[0];
        });
      }
    )

  }



  onPowerSwitchChange1(event) {
    console.log('onPowerSwitchChange');
    let value = this.power1 ? 1 : 0;
    let buffer = new Uint8Array([value]).buffer;
    console.log('Power Switch Property ' + this.power1);
    this.ble.write(this.peripheral.id, this.peripheral.characteristics[0].service, this.peripheral.characteristics[0].characteristic, buffer).then(
      () => this.setStatus('Light is ' + (this.power1 ? 'on' : 'off')),
      e => this.showAlert('Unexpected Error', 'Error updating power switch')
    );
  }

  onPowerSwitchChange2(event) {
    console.log('onPowerSwitchChange');
    let value = this.power2 ? 1 : 0;
    let buffer = new Uint8Array([value]).buffer;
    console.log('Power Switch Property ' + this.power2);
    this.ble.write(this.peripheral.id, this.peripheral.characteristics[1].service, this.peripheral.characteristics[1].characteristic, buffer).then(
      () => this.setStatus('Light is ' + (this.power2 ? 'on' : 'off')),
      e => this.showAlert('Unexpected Error', 'Error updating power switch')
    );
  }

  setDC1(event) {
    let buffer = new Uint8Array([this.DC1]).buffer;
    this.ble.write(this.peripheral.id, this.peripheral.characteristics[2].service, this.peripheral.characteristics[2].characteristic, buffer).then(
      () => this.setStatus('Set brightness to ' + this.DC1),
      e => this.showAlert('Unexpected Error', 'Error updating dimmer characteristic ' + e)
    );
  }

  setDC2(event) {
    let buffer = new Uint8Array([this.DC2]).buffer;
    this.ble.write(this.peripheral.id, this.peripheral.characteristics[3].service, this.peripheral.characteristics[3].characteristic, buffer).then(
      () => this.setStatus('Set brightness to ' + this.DC2),
      e => this.showAlert('Unexpected Error', 'Error updating dimmer characteristic ' + e)
    );
  }

  // Disconnect peripheral when leaving the page
  ionViewWillLeave() {
    console.log('ionViewWillLeave disconnecting Bluetooth');
    this.ble.disconnect(this.peripheral.id).then(
      () => console.log('Disconnected ' + JSON.stringify(this.peripheral)),
      () => console.log('ERROR disconnecting ' + JSON.stringify(this.peripheral))
    )
  }

  showAlert(title, message) {
    let alert = this.alertCtrl.create({
      title: title,
      subTitle: message,
      buttons: ['OK']
    });
    alert.present();
  }

  setStatus(message) {
    console.log(message);
    this.ngZone.run(() => {
      this.statusMessage = message;
    });
  }
}
